#pragma once

namespace Eyestack::Base {
class Profiler;
}

/**
 * @brief 标准输出流打印函数，以缩进文本的方式打印输出。
 */
std::ostream&
operator<<(std::ostream& out, const Eyestack::Base::Profiler& obj);

/**
 * @brief 序列化函数。
 */
QDataStream&
operator<<(QDataStream& ds, const Eyestack::Base::Profiler& obj);

/**
 * @brief 反序列化函数。
 */
QDataStream&
operator>>(QDataStream& ds, Eyestack::Base::Profiler& obj);

namespace Eyestack::Base {

/**
 * @brief 分析记录器
 */
class EYESTACK_BASE_EXPORT Profiler
{
public:
  using Deleter = std::function<void(void*)>;
  using Clock = std::chrono::high_resolution_clock;

  /**
   * @brief 分析记录条目类
   */
  struct Entry
  {
    Entry(Clock::time_point time, const char* tag)
      : _time(time)
      , _tag(tag)
    {}
    Clock::time_point _time;
    const char* _tag;
  };

  class Scoper;

public:
  Profiler() = default;
  Profiler(Profiler&& other) noexcept;
  ~Profiler() noexcept;

public:
  /**
   * @brief 计时函数
   * @param tag 计时标签。使用指针是为了尽可能避免开销，调用者需保证在 Profiler
   * 生命期内指针始终有效，如果不方便管理指针，可以使用下面的 give_ptr 把字符串
   * 交给 Profiler 托管。
   */
  void time(const char* tag) noexcept
  {
    _entries.emplace_back(gClock.now(), tag);
  }

  /**
   * @brief 获取记录条目清单
   * @return 条目清单的引用，可以自由修改。
   */
  std::vector<Entry>& entries() { return _entries; }

  /**
   * @brief 清空记录条目，释放所有托管指针
   */
  void clear();

  /**
   * @brief 将指针交给 Profiler 托管，若指针已存在则更新释放器
   * @param ptr 要托管的指针，移交语义
   * @param deleter 释放器，接受托管的指针为参数
   */
  void give_ptr(void* ptr, Deleter deleter) noexcept { _ptrmgr[ptr] = deleter; }

  /**
   * @brief 从 Profiler 取回托管的指针
   * @param ptr 要取回的指针
   * @return 当初设置的释放器，若没有找到指针，返回空的释放器
   */
  Deleter take_ptr(void* ptr) noexcept
  {
    auto it = _ptrmgr.find(ptr);
    if (it == _ptrmgr.end())
      return Deleter();
    auto retval = it->second;
    _ptrmgr.erase(it);
    return retval;
  }

private:
  static const Clock gClock;

private:
  std::vector<Entry> _entries;
  std::map<void*, Deleter> _ptrmgr;

private:
  friend std::ostream& ::operator<<(std::ostream& out, const Profiler& obj);
  friend QDataStream& ::operator<<(QDataStream& ds, const Profiler& obj);
  friend QDataStream& ::operator>>(QDataStream& ds, Profiler& obj);
};

/**
 * @brief 作用域计时类，在构造时进行一次计时，在析构时自动进行一次计时
 */
class Profiler::Scoper
{
  Scoper(const Scoper&) = delete;
  Scoper(Scoper&&) = delete;
  Scoper& operator=(const Scoper&) = delete;
  Scoper& operator=(Scoper&&) = delete;

public:
  Scoper(Profiler& self, const char* tag)
    : _self(self)
    , _tag(tag)
  {
    _self.time(_tag);
  }
  ~Scoper() { _self.time(_tag); }

private:
  Profiler& _self;
  const char* _tag;
};

}

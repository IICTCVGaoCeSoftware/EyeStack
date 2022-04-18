#include "Eyestack/Base/Profiler.hpp"

namespace sc = std::chrono;

template<typename T1, typename T2>
static std::ostream&
operator<<(std::ostream& out, const sc::duration<T1, T2>& dura)
{
  auto count = sc::duration_cast<sc::nanoseconds>(dura).count();

  if (count < 10000)
    out << count << "ns";
  else if ((count /= 1000) < 10000)
    out << count << "us";
  else if ((count /= 1000) < 10000)
    out << count << "ms";
  else
    out << count / 1000 << "s";

  return out;
}

std::ostream&
operator<<(std::ostream& out, const Eyestack::Base::Profiler& obj)
{
  using namespace Eyestack::Base;

  std::vector<Profiler::Entry> stack;
  stack.emplace_back(Profiler::Clock::time_point(), nullptr);

  for (auto& i : obj._entries) {
    // 调用返回
    if (i._tag == stack.back()._tag) {
      for (int i = 2; i < stack.size(); ++i)
        out << '\t';
      out << i._tag << " [" << (i._time - stack.back()._time) << "]\n";
      stack.pop_back();
    }

    // 调用进入
    else {
      for (int i = 1; i < stack.size(); ++i)
        out << '\t';
      out << i._tag << '\n';
      stack.push_back(i);
    }
  }

  return out;
}

static void
delete_chars(void* p)
{
  delete[] reinterpret_cast<char*>(p);
}

QDataStream&
operator<<(QDataStream& ds, const Eyestack::Base::Profiler& obj)
{
  std::set<const char*> strs;

  for (auto&& i : obj._entries) {
    // 如果标签字符串没有序列化，先序列化标签字符串
    if (strs.find(i._tag) == strs.end()) {
      strs.insert(i._tag);
      ds << size_t(i._tag) << i._tag;
    }

    // 序列化记录条目
    ds << size_t(i._tag) << i._time.time_since_epoch().count();
  }

  // 以一个与 i._tag 同宽度的 0 作为结束字符
  ds << size_t(0);

  return ds;
}

QDataStream&
operator>>(QDataStream& ds, Eyestack::Base::Profiler& obj)
{
  obj.clear();

  std::map<size_t, char*> strs;

  while (true) {
    size_t key;
    ds >> key;

    // 如果 key 是 0，表明已经读完了
    if (key == 0)
      break;

    // 没遇见过的 key 后紧跟标签字符串
    auto it = strs.find(key);
    if (it == strs.end()) {
      char* str;
      ds >> str;
      strs[key] = str;

      // 将读取的字符串交给 Profiler 托管
      obj._ptrmgr[str] = &delete_chars;
    }

    // 遇见过的 key 后面紧跟计时次数
    else {
      using Clock = Eyestack::Base::Profiler::Clock;
      Clock::duration::rep count;
      ds >> count;
      obj._entries.emplace_back(Clock::time_point(Clock::duration(count)),
                                it->second);
    }
  }

  return ds;
}

namespace Eyestack::Base {

Profiler::Profiler(Profiler&& other) noexcept
  : _entries(std::move(other._entries))
  , _ptrmgr(std::move(other._ptrmgr))
{}

Profiler::~Profiler() noexcept
{
  for (auto&& i : _ptrmgr)
    i.second(i.first);
}

void
Profiler::clear()
{
  _entries.clear();
  for (auto&& i : _ptrmgr)
    i.second(i.first);
  _ptrmgr.clear();
}

namespace sc = std::chrono;

template<typename T1, typename T2>
static std::ostream&
operator<<(std::ostream& out, const sc::duration<T1, T2>& dura)
{
  long long count;

  if ((count = sc::duration_cast<sc::nanoseconds>(dura).count()) < 10000)
    out << count << "ns";
  else if ((count = sc::duration_cast<sc::microseconds>(dura).count()) < 10000)
    out << count << "us";
  else if ((count = sc::duration_cast<sc::milliseconds>(dura).count()) < 10000)
    out << count << "ms";
  else
    out << sc::duration_cast<sc::seconds>(dura).count() << "s";

  return out;
}

}

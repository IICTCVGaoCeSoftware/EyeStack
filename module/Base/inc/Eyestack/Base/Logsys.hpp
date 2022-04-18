#pragma once

#include "Profiler.hpp"

namespace Eyestack::Base {

// ==========================================================================
// Logsys
// ==========================================================================

/**
 * @brief 单例模式的日志系统类。
 *
 * 具备输出重定向、目录大小监控、归档管理等功能。
 */
class EYESTACK_BASE_EXPORT Logsys
  : private QObject
  , private QRunnable
{
  Q_OBJECT
  friend class QObject;

  using _T = Logsys;
  using _S = QObject;

public:
  /**
   * @brief 配置清单结构体
   */
  struct Manifest
  {
    /**
     * @brief 运行日志目录，必须以“/”结尾，如果不存在则创建，默认为“log/”。
     */
    QString logPath{ "log/" };

    /**
     * @brief 日志归档目录，必须以“/”结尾，如果不存在则创建，默认为“log.old”。
     */
    QString arcPath{ "log.old/" };

    /**
     * @brief 存储大小限制，单位字节，默认为 2 GB。
     */
    size_t storageLimit{ 1u << 31 };

    /**
     * @brief 存储检查时间间隔，单位毫秒，默认为 1 小时。
     */
    int checkInterval{ 60 * 60 * 1000 };

    /**
     * @brief 是否禁用框架定义 Qt 日志输出。
     */
    bool noMessageHandler{ false };

    /**
     * @brief 是否禁用重定向，默认为 false。
     */
    bool noRedirect{ false };

    /**
     * @brief 是否禁用 C 信号处理。
     */
    bool noSignalHandler{ false };
  };

public:
  /**
   * @brief 获取全局实例，如果还没创建，返回 nullptr。
   */
  static Logsys* instance() { return sInstance; }

public:
  /**
   * @brief 构造函数
   *
   * 构造函数中不会检查目录是否已超过大小限制。
   *
   * @param manifest 配置清单对象，必须保证在对象的整个生命期内可用。
   *
   * @throw 除非日志系统正常启动（所依赖的目录存在、重定向成功等），否则
   * 就会抛出 exc::InitializationError 异常。
   */
  Logsys(Manifest& manifest) noexcept(false);

  /**
   * @brief 退出时自动将 logPath 目录用时间戳归档到 arcPath 下。
   */
  ~Logsys() noexcept;

public:
  /**
   * @brief 获取初始化时的配置清单
   */
  const Manifest& manifest() noexcept { return _manifest; }

  /**
   * @brief 遍历目录，统计日志存储大小，这可能会很耗时
   * @return 字节数
   */
  size_t count();

  /**
   * @brief 获取日志线程池
   */
  QThreadPool& thread_pool() { return _logtp; }

signals:
  /**
   * @brief 当检测到日志目录大小超限时发出此信号。
   * @param limit 当前大小限制，单位字节
   * @param current 检测到的目录大小，单位字节
   */
  void s_oversize(size_t limit,
                  size_t current,
                  QPrivateSignal = QPrivateSignal());

private:
  static Logsys* sInstance;

private:
  Manifest& _manifest;
  QThreadPool _logtp;

  // QObject interface
private:
  virtual void timerEvent(QTimerEvent* event) override;

  // QRunnable interface
public:
  virtual void run() override;
};

// ==========================================================================
// Logger
// ==========================================================================

/**
 * @brief 支持运行时禁用输出的异步日志助手类
 *
 * 已经内置了一些对常见类型的支持，同时也支持子类继承以添加新的日志类型。
 */
class EYESTACK_BASE_EXPORT Logger
{
public:
  /**
   * @brief 构造函数
   * @param prefix 日志文件前缀，默认为空。
   */
  Logger(const QString& prefix = QString())
    : _prefix(Logsys::instance()->manifest().logPath + prefix)
    , _logtp(&Logsys::instance()->thread_pool())
  {
    Q_ASSERT(prefix.endsWith("/"));
  }

public:
  /**
   * @brief 获取日志路径前缀
   */
  const QString& get_prefix() { return _prefix; }

  /**
   * @brief 设置日志路径前缀
   * @param prefix 路径字符串，必须以“/”结尾
   */
  void set_prefix(const QString& prefix)
  {
    Q_ASSERT(prefix.endsWith("/"));
    _prefix = prefix;
  }

  /**
   * @brief 启用对象
   *
   * 等同于 logger.set_thread_pool();
   */
  void enable() { _logtp = &Logsys::instance()->thread_pool(); }

  /**
   * @brief 禁用对象
   *
   * 等同于 logger.set_thread_pool(nullptr);
   */
  void disable() { _logtp = nullptr; }

  /**
   * @brief 通用异步日志方法
   * @param subpath 日志文件子路径
   * @param logf 日志函数，接受一个打开的 QDataStream 引用参数，不可抛出异常。
   */
  void alog(const QString& subpath, std::function<void(QDataStream&)> logf);

protected:
  QString _prefix;     ///< 日志目录前缀
  QThreadPool* _logtp; ///< 日志线程池，为空表示对象被禁用

  // ======================================================================
  // ↓ 预置日志类型支持 ↓
  // ======================================================================

public:
  /**
   * @brief 用于拓展日志类型支持的方便基类
   */
  class LoggerBase
  {
    friend class Logger;

  protected:
    /**
     * @brief 创建路径的父目录
     * @param path 路径
     * @return 成功返回 true，失败返回 false
     */
    static bool mkparent(const QString& path) noexcept
    {
      return QDir().mkpath(path.left(path.lastIndexOf('/')));
    }

  public:
    LoggerBase(QThreadPool* logtp, const QString& path)
      : _logtp(logtp)
      , _path(path)
    {}

  protected:
    QThreadPool* _logtp; ///< 日志线程池，为 nullptr 表示禁用
    QString _path;       ///< 目标日志文件路径
  };

  class ProfilerLogger : public LoggerBase
  {
  public:
    using LoggerBase::LoggerBase;

  public:
    void operator<<(Profiler& prof);
  };

  ProfilerLogger profiler(const QString& subpath) const
  {
    return ProfilerLogger(_logtp, _prefix + subpath);
  }

  /**
   * @details 一个图像文件只能存储一张图像，所以返回值设计为 void。
   */
  class ImageLogger : public LoggerBase
  {
  public:
    using LoggerBase::LoggerBase;

  public:
    void operator<<(const QImage& img);
    void operator<<(const cv::Mat& mat);
  };

  /**
   * @brief 记录图像日志
   * @param subpath 文件子路径，通过后缀指明文件格式（例如 .png）
   */
  ImageLogger image(const QString& subpath) const
  {
    return ImageLogger(_logtp, _prefix + subpath);
  }
};

}

Q_DECLARE_METATYPE(::cv::Mat)

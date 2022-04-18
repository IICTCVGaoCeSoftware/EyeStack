#include "Eyestack/Base/Logsys.hpp"
#include "Eyestack/Base/exc.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <csignal>

namespace Eyestack::Base {

static const QLoggingCategory sLogcat("Logsys");

// ==========================================================================
// Logsys
// ==========================================================================

Logsys* Logsys::sInstance = nullptr;

static FILE* sFallbackFile = nullptr;

static void
message_handler(QtMsgType type,
                const QMessageLogContext& context,
                const QString& msg)
{
  const char* typestr;
  switch (type) {
    case QtDebugMsg:
      typestr = "Debug";
      break;
    case QtInfoMsg:
      typestr = "Info";
      break;
    case QtWarningMsg:
      typestr = "Warning";
      break;
    case QtCriticalMsg:
      typestr = "Critical";
      break;
    case QtFatalMsg:
      typestr = "Fatal";
      break;
  }

  // 打印当前时间
  using namespace boost::posix_time;
  std::ostringstream oss;
  oss << microsec_clock::local_time();

  // 打印非结构化记录体
  QByteArray localMsg = msg.toLocal8Bit();

#ifdef _DEBUG
  fprintf(sFallbackFile,
          "\n<QThread %p> [%s %s %s %u] \"%s\" \"%s\" %u\n%s\n",
          QThread::currentThread(),
          oss.str().c_str(),
          typestr,
          context.category,
          context.version,
          context.function,
          context.file,
          context.line,
          localMsg.constData());
#else
  fprintf(sFallbackFile,
          "\n<QThread %p> [%s %s %s %u]\n%s\n",
          QThread::currentThread(),
          oss.str().c_str(),
          typestr,
          context.category,
          context.version,
          localMsg.constData());
#endif

  if (type == QtFatalMsg)
    abort();
}

static void
signal_handler(int sig)
{
  auto& tp = Logsys::instance()->thread_pool();
  if (!tp.contains(QThread::currentThread()))
    tp.waitForDone();
}

Logsys::Logsys(Manifest& manifest) noexcept(false)
  : _manifest(manifest)
{
  Q_ASSERT(manifest.logPath.endsWith('/'));
  Q_ASSERT(manifest.arcPath.endsWith('/'));

  Q_ASSERT(sInstance == nullptr);
  sInstance = this;

  // 创建日志系统目录
  {
    if (!QDir(manifest.logPath).exists() && !QDir().mkdir(manifest.logPath))
      throw exc::InitializationError(
        tr("无法打开日志目录 \"%1\"。").arg(manifest.logPath));
    if (!QDir(manifest.arcPath).exists() && !QDir().mkdir(manifest.arcPath))
      throw exc::InitializationError(
        tr("无法打开归档目录 \"%1\"。").arg(manifest.arcPath));
  }

  // QtDebug 输出格式化
  if (!manifest.noMessageHandler) {
    qInstallMessageHandler(&message_handler);
  }

  // 重定向
  if (!manifest.noRedirect) {
    auto fallbackPath = manifest.logPath + "fallback.log";
    sFallbackFile = fopen(fallbackPath.toStdString().c_str(), "a");
    if (sFallbackFile == nullptr)
      throw exc::InitializationError(
        tr("无法打开回退文件 \"%1\"。").arg(fallbackPath));

    auto stdoutPath = manifest.logPath + "stdout.log";
    if (freopen(stdoutPath.toStdString().c_str(), "a", stdout))
      qInfo(sLogcat) << "已重定向 stdout 到文件" << stdoutPath << "。";
    else
      qCritical(sLogcat) << "重定向 stdout 到文件" << stdoutPath << "失败。";

    auto stderrPath = manifest.logPath + "stderr.log";
    if (freopen(stderrPath.toStdString().c_str(), "a", stderr))
      qInfo(sLogcat) << "已重定向 stderr 到文件" << stderrPath << "。";
    else
      qCritical(sLogcat) << "重定向 stderr 到文件" << stderrPath << "失败。";
  } else {
    sFallbackFile = stderr;
  }

  // 注册信号处理函数
  if (!manifest.noSignalHandler) {
    signal(SIGTERM, signal_handler);
    signal(SIGSEGV, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGILL, signal_handler);
    signal(SIGABRT, signal_handler);
    signal(SIGFPE, signal_handler);
  }

  // 启动存储检查定时器
  startTimer(manifest.checkInterval);
}

Logsys::~Logsys() noexcept
{
  _logtp.waitForDone();

  if (!_manifest.noRedirect) {
    fclose(sFallbackFile);
  }

  auto timestr = QDateTime::currentDateTime().toString("yyyy-MM-dd hh.mm.ss");
  QDir().rename(_manifest.logPath, _manifest.arcPath + timestr);
}

size_t
Logsys::count()
{
  qint64 size = 0;

  {
    QDirIterator it(_manifest.logPath, QDirIterator::Subdirectories);
    while (it.hasNext()) {
      it.next();
      size += it.fileInfo().size();
    }
  }

  {
    QDirIterator it(_manifest.arcPath, QDirIterator::Subdirectories);
    while (it.hasNext()) {
      it.next();
      size += it.fileInfo().size();
    }
  }

  return size;
}

void
Logsys::timerEvent(QTimerEvent* event)
{
  _logtp.start(this);
}

void
Logsys::run()
{
  qInfo() << "定时日志存储检查启动，当前检查时间间隔" << _manifest.checkInterval
          << "毫秒。";
  QElapsedTimer et;
  et.start();
  auto limit = _manifest.storageLimit;
  auto size = count();
  if (size > limit)
    emit s_oversize(limit, size);
  auto elapsed = et.elapsed();
  qInfo() << "日志存储已使用" << size << "字节（上限" << limit
          << "字节），统计耗时" << elapsed << "微秒";
}

void
Logger::ProfilerLogger::operator<<(Profiler& prof)
{
  if (_logtp == nullptr)
    return;

  auto& path = _path;
  auto ptr = new Profiler(std::move(prof));

  _logtp->start([path, ptr]() {
    mkparent(path);

    QFile file(path);
    if (!file.open(QFile::WriteOnly)) {
      qCritical(sLogcat) << "日志记录失败：无法保存 Base::Profiler 到" << path
                         << "。";
    } else {
      QDataStream ds(&file);
      ds << *ptr;
    }

    delete ptr;
  });
}

void
Logger::ImageLogger::operator<<(const QImage& img)
{
  if (_logtp == nullptr)
    return;

  auto& path = _path;
  _logtp->start([path, img]() {
    mkparent(path);
    if (!img.save(path))
      qCritical(sLogcat) << "日志记录失败：无法保存 QImage 到" << path << "。";
  });
}

void
Logger::ImageLogger::operator<<(const cv::Mat& mat)
{
  if (_logtp == nullptr)
    return;

  auto& path = _path;
  _logtp->start([path, mat]() {
    mkparent(path);
    if (!cv::imwrite(path.toStdString(), mat))
      qCritical(sLogcat) << "日志记录失败：无法保存 cv::Mat 到" << path << "。";
  });
}

void
Logger::alog(const QString& subpath, std::function<void(QDataStream&)> logf)
{
  if (_logtp == nullptr)
    return;

  auto path = _prefix + subpath;
  _logtp->start([path, logf]() {
    LoggerBase::mkparent(path);
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
      qCritical(sLogcat) << "日志记录失败：无法为日志函子打开文件" << path
                         << "。";
      return;
    }

    QDataStream ds(&file);
    logf(ds);
  });
}

}

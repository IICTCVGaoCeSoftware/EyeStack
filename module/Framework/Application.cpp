#include "Eyestack/Framework/Application.hpp"
#include "Eyestack/Asset.hpp"

using namespace Eyestack::Base::exc;

namespace Eyestack::Framework {

using namespace Base::util;

static QLoggingCategory sLogcat("Application");

Application::~Application()
{
  qInfo(sLogcat) << "系统已退出。";
}

int
Application::exec()
{
  qInfo(sLogcat) << "系统运行已启动。";

  _splash.finish(_mainWindow.data());

#ifndef _DEBUG
  try {
#endif

    _mainWindow->load_prefs(".espref");
    _mainWindow->show();
    auto retval = _S::exec();

    qInfo(sLogcat) << "系统运行正常结束。";
    return retval;

#ifndef _DEBUG
  } catch (const Base::exc::Exception& e) {
    auto repr = e.repr();
    qCritical(sLogcat) << "主事件循环中抛出异常：" << repr;
    QMessageBox::critical(_mainWindow.data(), tr("系统崩溃！"), repr);

  } catch (const std::exception& e) {
    auto what = e.what();
    qCritical(sLogcat) << "主事件循环中抛出异常：" << what;
    QMessageBox::critical(_mainWindow.data(), tr("系统崩溃！"), what);

  } catch (...) {
    qCritical(sLogcat) << "主事件循环中抛出未知异常";
    QMessageBox::critical(_mainWindow.data(), tr("系统崩溃！"), tr("未知异常"));
  }
#endif

  return -1;
}

void
Application::init()
{
  // 加载资源文件并设置界面样式
  setStyle(QStyleFactory::create("fusion"));
  Asset::ginit();

  // 显示启动界面
  if (_manifest.splashFile.isNull())
    _splash.setPixmap(QPixmap(":/icon/eyestack.svg"));
  else
    _splash.setPixmap(QPixmap(_manifest.splashFile));
  _splash.show();

  // 设置应用属性
  if (!_manifest.name.isNull())
    setApplicationName(_manifest.name);
  if (!_manifest.version.isNull())
    setApplicationVersion(_manifest.version);
  setOrganizationName(_manifest.orgName);
  setOrganizationDomain(_manifest.orgDomain);

  // 初始化日志系统
  try {
    Base::Logsys::Manifest& logsysManifest = _manifest.logsys;
    for (auto& arg : arguments()) {
      if (arg == "--no-message-handler")
        logsysManifest.noMessageHandler = true;
      else if (arg == "--no-redirect")
        logsysManifest.noRedirect = true;
      else if (arg == "--no-signal-handler")
        logsysManifest.noSignalHandler = true;
    }

    _logsys.reset(new Base::Logsys(logsysManifest));
    connect(_logsys.data(),
            &Base::Logsys::s_oversize,
            this,
            &_T::when_logsys_oversize);

  } catch (Base::exc::InitializationError& e) {
    QMessageBox::critical(nullptr,
                          tr("日志系统初始化失败"),
                          tr("请修复以下错误后重新启动：\n\n") + e.repr());
    exit(-1);
  }

  // 初始化其它服务

  {
    _configurer.reset(new Framework::Configurer());
  }

  {
    _workctrl.reset(new Workctrl(*this));
  }

  {
    _mainWindow.reset(new MainWindow());

    if (_manifest.iconFile.isNull()) {
      _mainWindow->setWindowIcon(Asset::icon("eyestack"));
    } else {
      _mainWindow->setWindowIcon(QIcon(_manifest.iconFile));
    }

    connect(_mainWindow.data(),
            &MainWindow::cmd_reset_config,
            this,
            &_T::reset_config);
    connect(_mainWindow.data(),
            &MainWindow::cmd_export_config,
            this,
            &_T::export_config);
    connect(_mainWindow.data(),
            &MainWindow::cmd_import_config,
            this,
            &_T::import_config);

    connect(_mainWindow.data(),
            &MainWindow::cmd_start,
            this,
            [this](double freq) { _workctrl->start_worker(1000 / freq); });
    connect(_mainWindow.data(), &MainWindow::cmd_stop, this, [this]() {
      _workctrl->stop_worker();
    });
    connect(_mainWindow.data(), &MainWindow::cmd_pause, this, [this]() {
      _workctrl->pause_worker();
    });
    connect(_mainWindow.data(), &MainWindow::cmd_trigger, this, [this]() {
      _workctrl->trigger_worker();
    });
  }

  {
    Notifier::ginit();
    _notifier.reset(new Notifier(*_mainWindow));
  }

  {
    Bgtasker::ginit();
    _bgtasker.reset(new Bgtasker(*_mainWindow));
  }

  qInfo(sLogcat) << "系统初始化完毕。";
}

void
Application::when_logsys_oversize(size_t limit, size_t current)
{
  notifier().notify_text(
    tr("当前使用 %1 字节，上限 %2 "
       "字节。\n为避免系统运行出错，请清理日志目录 \"%3\"。")
      .arg(current)
      .arg(limit)
      .arg(logsys().manifest().arcPath, tr("警告：日志存储超限")),
    QtMsgType::QtWarningMsg);
}

void
Application::reset_config()
{
  _configurer->reset_configs();
}

void
Application::export_config(const QString& path)
{
  _configurer->dump_configs(path);
}

void
Application::import_config(const QString& path)
{
  _configurer->load_configs(path);
}

Application::Workctrl::Workctrl(Application& app)
  : _app(app)
{
  ginit();

  // 启动工作线程
  start();

  // 配置定时器，每秒刷新一次界面上的工频显示器
  _timer.setInterval(1000);
  app.connect(&_timer, &QTimer::timeout, [&app]() {
    auto worker = app._workctrl->get_worker();
    if (!worker)
      return;
    app._mainWindow->_statusBar._cycleLcd.display(worker->cost_time() /
                                                  1000000.0);
    app._mainWindow->_statusBar._frequencyLcd.display(1000.0 /
                                                      worker->cycle_time());
  });
}

void
Application::Workctrl::when_started()
{
  _timer.start();
  _app._mainWindow->set_state_started();
  _app._mainWindow->_statusBar._statusLabel.setText(tr("正在运行..."));
}

void
Application::Workctrl::when_stopped()
{
  _timer.stop();
  _app._mainWindow->set_state_stopped();
  _app._mainWindow->_statusBar._statusLabel.setText(tr("运行已停止"));
}

void
Application::Workctrl::when_paused()
{
  _app._mainWindow->set_state_paused();
  _app._mainWindow->_statusBar._statusLabel.setText(tr("运行已暂停"));
}

void
Application::Workctrl::when_failed(std::exception_ptr e)
{
  try {
    std::rethrow_exception(e);

  } catch (const Base::exc::Exception& e) {
    _app._mainWindow->_statusBar._statusLabel.setText(tr("运行出错"));
    _app._notifier->notify_text(
      e.repr(), QtMsgType::QtCriticalMsg, tr("工作控制服务"));

  } catch (const std::exception& e) {
    _app._mainWindow->_statusBar._statusLabel.setText(tr("运行出错"));
    _app._notifier->notify_text(
      e.what(), QtMsgType::QtCriticalMsg, tr("工作控制服务"));

  } catch (...) {
    _app._mainWindow->_statusBar._statusLabel.setText(tr("运行出错"));
    _app._notifier->notify_text(
      tr("未知错误"), QtMsgType::QtFatalMsg, tr("工作控制服务"));
  }
}

void
Application::MainWindow::closeEvent(QCloseEvent* event)
{
  dump_prefs(".espref");
  _S::closeEvent(event);
}

}

#pragma once

#include "Eyestack/Base/Logsys.hpp"
#include "Eyestack/Framework/Bgtasker.hpp"
#include "Eyestack/Framework/Configurer.hpp"
#include "Eyestack/Framework/MainWindow.hpp"
#include "Eyestack/Framework/Notifier.hpp"
#include "Eyestack/Framework/Workctrl.hpp"

namespace Eyestack::Framework {

/**
 * @brief 集成了所有框架功能、开箱即用的应用类。
 */
class EYESTACK_FRAMEWORK_EXPORT Application : public QApplication
{
  Q_OBJECT

  using _T = Application;
  using _S = QApplication;

public:
  /**
   * @brief 应用配置清单结构体，用于简化构造函数的参数表
   */
  struct Manifest
  {
    Manifest() {}

    /**
     * @brief 欢迎界面图片路径，如果为空则使用 Eyestack 图标
     */
    QString splashFile;

    /**
     * @brief 应用程序名，如果为空则使用默认应用名
     */
    QString name;

    /**
     * @brief 应用程序版本
     */
    QString version;

    /**
     * @brief 组织名
     */
    QString orgName;

    /**
     * @brief 组织域名
     */
    QString orgDomain;

    /**
     * @brief 应用程序图标文件路径，如果为空则使用 Eyestack 图标
     */
    QString iconFile;

    /**
     * @brief Logsys 配置清单结构体
     */
    Base::Logsys::Manifest logsys;
  };

public:
  /**
   * @brief 获取全局唯一的 Application 实例
   * @return 实例指针，如果没有创建实例，则返回 nullptr
   */
  static _T* instance()
  {
    return reinterpret_cast<_T*>(QApplication::instance());
  }

  /**
   * @brief 日志系统服务接口
   */
  static Base::Logsys& logsys()
  {
    Q_ASSERT(dynamic_cast<Application*>(QApplication::instance()));
    return *instance()->_logsys;
  }

  /**
   * @brief 配置管理器服务接口
   */
  static Framework::Configurer& configurer()
  {
    Q_ASSERT(dynamic_cast<Application*>(QApplication::instance()));
    return *instance()->_configurer;
  }

  /**
   * @brief 工作控制服务接口
   */
  static Framework::Workctrl& workctrl()
  {
    Q_ASSERT(dynamic_cast<Application*>(QApplication::instance()));
    return *instance()->_workctrl;
  }

  /**
   * @brief 主界面服务接口
   */
  static Framework::MainWindow& main_window()
  {
    Q_ASSERT(dynamic_cast<Application*>(QApplication::instance()));
    return *instance()->_mainWindow;
  }

  /**
   * @brief 通知管理器服务接口
   */
  static Framework::Notifier& notifier()
  {
    Q_ASSERT(dynamic_cast<Application*>(QApplication::instance()));
    return *instance()->_notifier;
  }

  /**
   * @brief 任务控制服务接口
   */
  static Framework::Bgtasker& bgtasker()
  {
    Q_ASSERT(dynamic_cast<Application*>(QApplication::instance()));
    return *instance()->_bgtasker;
  }

public:
  /**
   * @brief 构造方法
   * @param argc 命令行参数个数
   * @param argv 命令行参数向量
   * @param manifest 应用配置清单，必须保证在整个生命期内可用。
   */
  Application(int argc, char* argv[], Manifest manifest = Manifest())
    : QApplication(argc, argv)
    , _manifest(std::move(manifest))
  {
    init();
  }

  ~Application();

public:
  /**
   * @brief 重载基类的 exec 方法
   */
  int exec();

  /**
   * @brief 注册配置
   *
   * @param uuid 配置对象的唯一标识符
   * @param config 配置对象
   */
  void reg_config(uint64_t uuid, Framework::Configurer::Config& config)
  {
    _configurer->register_config(uuid, config);
  }

  /**
   * @brief 获取应用的工作对象
   * @return 指针为借用语义，如果没有设置工作对象，返回 nullptr
   */
  Worker* worker() { return _worker.get(); }

  /**
   * @brief 注册工作对象，只能注册一个对象
   * @param worker 移交语义
   */
  void reg_worker(Worker* worker)
  {
    Q_ASSERT(_worker.isNull());
    _worker.reset(worker);
    _workctrl->set_worker(worker);
  }

  /**
   * @brief 注册子界面对象
   * @param subui 子界面对象
   */
  void reg_sub_ui(Framework::MainWindow::SubUi& subui)
  {
    _mainWindow->register_sub_ui(subui);
  }

  /**
   * @brief 注册配置向导
   * @param wizard 配置向导对象
   */
  void reg_config_wizard(QDialog& wizard)
  {
    _mainWindow->register_config_wizard(wizard);
  }

private:
  class Workctrl : public Framework::Workctrl
  {
  public:
    Workctrl(Application& app);

  private:
    Application& _app;
    QTimer _timer; // 用于定时刷新界面上的工频显示

    // Workctrl interface
  public:
    virtual void when_started() override;
    virtual void when_stopped() override;
    virtual void when_paused() override;
    virtual void when_failed(std::exception_ptr e) override;
  };

  class MainWindow : public Framework::MainWindow
  {
    using _T = MainWindow;
    using _S = Framework::MainWindow;

  public:
    using _S::_S;

    // QWidget interface
  protected:
    virtual void closeEvent(QCloseEvent* event) override;
  };

private:
  Manifest _manifest;
  QSplashScreen _splash;

  QScopedPointer<Base::Logsys> _logsys;
  QScopedPointer<Configurer> _configurer;

  QScopedPointer<Worker> _worker; // 必须声明在 _workctrl 之前
  QScopedPointer<Workctrl> _workctrl;

  QScopedPointer<MainWindow> _mainWindow;
  QScopedPointer<Notifier> _notifier;
  QScopedPointer<Bgtasker> _bgtasker;

private:
  void init();

private slots:
  void when_logsys_oversize(size_t limit, size_t current);
  void reset_config();
  void export_config(const QString& path);
  void import_config(const QString& path);
};

}

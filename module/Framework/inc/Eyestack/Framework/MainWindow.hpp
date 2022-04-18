#pragma once

#include "Eyestack/Asset.hpp"
#include "Eyestack/Base.hpp"
#include "Eyestack/Design/AboutEyestack.hpp"
#include "Eyestack/Design/ListScroll.hpp"
#include "Eyestack/Design/StyleWindow.hpp"

namespace Eyestack::Framework {

/**
 * @brief 主界面类
 *
 * 在设计上，该类支持两种使用方式：直接实例化使用，或子类继承以进行界面定制。
 *
 * 1. 直接实例化
 *
 *    该类实现了基本的界面逻辑，将所有用户在主界面上的操作转化为命名以 “cmd_”
 *    开头的 Qt 信号，从而实现了程序界面和内部逻辑的解耦，在实例化后将这些信号
 *    连接到相应的处理槽即可。
 *
 * 2. 子类继承定制
 *
 *    主界面上的所有组件都对子类开放操作，从而允许了子类最大限度的定制需求。在
 *    子类中操作主界面对象时务必小心，请确保自己对主界面组件上已经包含的逻辑
 *    有足够的了解，尤其是在改变界面上已有组件的 _parent 时！
 */
class EYESTACK_FRAMEWORK_EXPORT MainWindow : public QMainWindow
{
  Q_OBJECT

  using _T = MainWindow;
  using _S = QMainWindow;

public:
  class SubUi;

public:
  // 下面这些类中只应该写初始化和状态设定函数这些代码，对外体现为完全被动的输出组件。

  struct MenuBar : public QMenuBar
  {
    struct Config : public QMenu
    {
      QAction _wizard;
      QAction _reset;
      QAction _import;
      QAction _export;

    public:
      Config(QWidget* parent = nullptr);

    public:
      /**
       * @brief 设定是否允许用户写入修改
       * @param yes 若是，则启用向导、重置、导入三个动作
       */
      void allow_modification(bool yes)
      {
        _wizard.setEnabled(yes);
        _reset.setEnabled(yes);
        _import.setEnabled(yes);
      }
    } _config;

    struct Run : public QMenu
    {
      QAction _stop;
      QAction _start;
      QAction _pause;
      QAction _trigger;

    public:
      Run(QWidget* parent = nullptr);
    } _run;

    struct View : public QMenu
    {
      QAction _cascadeSubWindows;
      QAction _tileSubWindows;

    public:
      View(QWidget* parent = nullptr);
    } _view;

    struct Help : public QMenu
    {
      QAction _aboutEystack;

    public:
      Help(QWidget* parent = nullptr);
    } _help;

  public:
    MenuBar(QWidget* parent = nullptr);
  } _menuBar;

  Design::AboutEyestack _aboutEyestack{ this };

  QToolBar _configurerTools;

  struct WorkctrlTools : public QToolBar
  {
    QDoubleSpinBox _frequencySpin;
    QToolButton _startButton;
    QToolButton _pauseButton;

  public:
    WorkctrlTools(QWidget* parent = nullptr);

  public:
    /**
     * @brief 切换停止状态
     */
    void switch_stopped();

    /**
     * @brief 切换运行状态
     */
    void switch_started();

    /**
     * @brief 切换暂停状态
     */
    void switch_paused();
  } _workctrlTools;

  QToolBar _subUiPanel;

  QMdiArea _mdiCenter;

  struct StatusBar : public QStatusBar
  {
    QToolButton _taskctrlButton;
    QLabel _statusLabel;
    QLabel _cycleLabel;
    QLCDNumber _cycleLcd;
    QLabel _frequencyLabel;
    QLCDNumber _frequencyLcd;
    QToolButton _notifierButton;

  public:
    StatusBar(QWidget* parent = nullptr);
  } _statusBar;

  struct TaskctrlWindow : public Design::StyleWindow
  {
    // 标题栏
    QHBoxLayout _titleLayout;
    QSizeGrip _sizeGrip{ this };
    QLabel _titleLabel;
    QToolButton _hideButton;

    // 主体区
    Design::ListScroll _listScroll;

  public:
    TaskctrlWindow(QWidget* parent);

  protected:
    virtual void showEvent(QShowEvent* event) override;
    virtual void hideEvent(QHideEvent* event) override;
  } _taskctrlWindow{ this };

  struct NotifierWindow : public Design::StyleWindow
  {
    // 标题栏
    QHBoxLayout _titleLayout;
    QSizeGrip _sizeGrip{ this };
    QLabel _titleLabel;
    QToolButton _clearButton;
    QToolButton _hideButton;

    // 主体区
    Design::ListScroll _listScroll;

  public:
    NotifierWindow(QWidget* parent);

  protected:
    virtual void showEvent(QShowEvent* event) override;
    virtual void hideEvent(QHideEvent* event) override;
  } _notifierWindow{ this };

public:
  MainWindow(QWidget* parent = nullptr);

public:
  /**
   * @brief 注册子界面
   * @param subUi 组件界面对象
   */
  void register_sub_ui(SubUi& subUi);

  /**
   * @brief 注册配置向导，目前仅支持注册一个向导，重复注册的行为未定义。
   * @param wizard 向导对话框控件的引用
   */
  void register_config_wizard(QDialog& wizard);

public:
  /**
   * @brief 重置界面显示偏好
   */
  void reset_prefs() noexcept;

  /**
   * @brief 保存界面显示偏好到文件
   * @param path 文件路径
   * @return 成功返回 true
   */
  bool dump_prefs(const QString& path) noexcept;

  /**
   * @brief 加载界面显示偏好到文件，如果失败则重置为默认值
   * @param path 文件路径
   * @return 成功返回 true
   */
  bool load_prefs(const QString& path) noexcept;

  /**
   * @brief 设置界面为运行状态
   */
  void set_state_started();

  /**
   * @brief 设置界面为暂停状态
   */
  void set_state_paused();

  /**
   * @brief 设置界面为停止状态
   */
  void set_state_stopped();

signals:
  /**
   * @brief 重置配置命令
   */
  void cmd_reset_config();

  /**
   * @brief 导入配置命令
   * @param path 配置文件路径
   */
  void cmd_import_config(const QString& path);

  /**
   * @brief 导出配置命令
   * @param path 配置文件路径
   */
  void cmd_export_config(const QString& path);

  /**
   * @brief 运行启动命令
   * @param freq 目标工作频率，单位赫兹
   */
  void cmd_start(double freq);

  /**
   * @brief 运行停止命令
   */
  void cmd_stop();

  /**
   * @brief 运行暂停命令
   */
  void cmd_pause();

  /**
   * @brief 触发命令
   */
  void cmd_trigger();

private:
  enum RunningState
  {
    kStopped,
    kStarted,
    kPaused
  } _runningState;

  QDialog* _configWizard{ nullptr };
  QVector<SubUi*> _subUis;

private slots:
  /**
   * @brief 更新通知和后台任务两个悬浮框的位置
   */
  void update_the_two_position();

  // QWidget interface
protected:
  virtual void showEvent(QShowEvent* event) override;
  virtual void hideEvent(QHideEvent* event) override;
  virtual void moveEvent(QMoveEvent* event) override;
  virtual void resizeEvent(QResizeEvent* event) override;
};

/**
 * @brief 子界面接口
 *
 * 用于在主界面显示和退出时加载和保存窗口的一些属性，例如窗口的长宽等。
 *
 * @see load_prefs dump_prefs
 */
class MainWindow::SubUi
{
public:
  class MdiAction;
  class MdiMenu;

public:
  /**
   * @brief 在主窗体上布置子界面，接口仅要求对于第一次调用有效。
   */
  virtual void setup_ui(MainWindow& mw) noexcept = 0;

  /**
   * @brief 重置界面显示偏好
   */
  virtual void reset_prefs() noexcept = 0;

  /**
   * @brief 保存界面显示偏好
   * @param ds 已打开的输出数据流
   * @return 成功返回 true
   */
  virtual bool dump_prefs(QDataStream& ds) noexcept = 0;

  /**
   * @brief 加载界面显示偏好
   *
   * 如果成功，则读取的字节数必须完全等于保存时写入的字节数。
   *
   * @param ds 已打开的输入数据流
   * @return 成功返回 true
   */
  virtual bool load_prefs(QDataStream& ds) noexcept = 0;
};

/**
 * @brief MainWindow 的预设子界面拓展方式之一，用于将自定义的 QWidget
 * 包装为 MDI 子窗体。
 *
 * 与 QMdiWindow 的区别在于：
 *
 * 1. 创建一个 QAction 控制子窗体的显示与否。
 * 2. 它不持有 setWidget 传入对象的所有权，如果在析构时有内部控件，控件的 parent
 *    将被置为 nullptr。
 */
class MainWindow::SubUi::MdiAction
  : public QMdiSubWindow
  , public MainWindow::SubUi
{
  using _T = MdiAction;
  using _S = QMdiSubWindow;

public:
  /**
   * @brief 关联的 QAction 对象
   */
  QAction _action;

public:
  MdiAction(const QString& name = QString(),
            const QIcon& icon = Asset::faicon("cube-solid"));
  ~MdiAction();

  // MainWindow::SubUi interface
public:
  virtual void setup_ui(MainWindow& mw) noexcept override;
  virtual void reset_prefs() noexcept override;
  virtual bool dump_prefs(QDataStream& ds) noexcept override;
  virtual bool load_prefs(QDataStream& ds) noexcept override;

  // QWidget interface
protected:
  virtual void showEvent(QShowEvent* event) override;
  virtual void hideEvent(QHideEvent* event) override;
};

/**
 * @brief MainWindow 的预设拓展方式之一，将多个 MdiAction 包装为菜单，
 * 设计用于实现比较复杂的组件界面。
 *
 * 将多个上层定义的 QWdiget 用 MdiAction 组织为菜单树，也可以往菜单中加入
 * 自定义的 QAction。
 */
class MainWindow::SubUi::MdiMenu
  : public QMenu
  , public MainWindow::SubUi
{
  using _T = MdiMenu;
  using _S = QMenu;

public:
  MdiMenu(const QString& name = QString(),
          const QIcon& icon = Asset::faicon("cubes-solid"));
  ~MdiMenu();

public:
  /**
   * @brief 包装一个自定义 QWidget 为 MdiAction，将其添加到菜单中。
   *
   * 必须在 setup_ui 之前调用，否则不保证显示在界面上。
   *
   * @param name 控件名称
   * @param widget 待包装控件
   * @param menu 目标菜单
   * @return 包装器对象的引用
   */
  MdiAction& add_mdi_action(const QString& name, QWidget& widget, QMenu& menu);

  /**
   * @brief 包装一个自定义 QWidget 为 MdiAction，将其添加到根菜单中。
   *
   * 必须在 setup_ui 之前调用，否则不保证显示在界面上。
   *
   * @param name 控件名称
   * @param widget 待包装的 QWidget
   * @return 包装器对象的引用
   */
  MdiAction& add_mdi_action(const QString& name, QWidget& widget)
  {
    return add_mdi_action(name, widget, *this);
  }

  // MainWindow::SubUi interface
public:
  virtual void setup_ui(MainWindow& mw) noexcept override;
  virtual void reset_prefs() noexcept override;
  virtual bool dump_prefs(QDataStream& ds) noexcept override;
  virtual bool load_prefs(QDataStream& ds) noexcept override;

private:
  QVector<MdiAction*> _mdiSubWindows;
};

}

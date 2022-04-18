#include "Eyestack/Framework/MainWindow.hpp"

namespace Eyestack::Framework {

using namespace Base::util;

constexpr auto faicon = Asset::faicon;

static const QLoggingCategory sLogcat("MainWindow");

// ==========================================================================
// MainWindow
// ==========================================================================

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
{
  setMenuBar(&_menuBar);
  {
    auto& config = _menuBar._config;

    connect(&config._wizard, &QAction::triggered, this, [this]() {
      _configWizard->exec();
    });

    connect(&config._reset, &QAction::triggered, this, [this]() {
      if (QMessageBox::question(this,
                                tr("重置默认配置"),
                                tr("确定重置所有配置为默认值？此操作无法撤销，"
                                   "这将丢失所有修改！")) != QMessageBox::Yes)
        return;
      emit cmd_reset_config();
    });

    connect(&config._import, &QAction::triggered, this, [this]() {
      QString fileName = QFileDialog::getOpenFileName(
        this, tr("导入文件"), QString(), tr("配置文件 (*.esconf)"));
      if (fileName.isEmpty())
        return;

      try {
        emit cmd_import_config(fileName);
        QMessageBox::information(
          this, tr("导入成功"), tr("已加载配置文件：\"%1\"").arg(fileName));

      } catch (Base::exc::LoadingError& e) {
        QMessageBox::critical(this, tr("导出失败"), e.repr());
      }
    });

    connect(&config._export, &QAction::triggered, this, [this]() {
      QString fileName = QFileDialog::getSaveFileName(
        this, tr("导出文件"), QString(), tr("配置文件 (*.esconf)"));
      if (fileName.isEmpty())
        return;

      try {
        emit cmd_export_config(fileName);
        QMessageBox::information(
          this, tr("导出成功"), tr("已转储配置文件：\"%1\"").arg(fileName));

      } catch (Base::exc::LoadingError& e) {
        QMessageBox::critical(this, tr("导入失败"), e.repr());
      }
    });

    auto& run = _menuBar._run;

    connect(&run._start, &QAction::triggered, this, [this]() {
      bool ok;
      auto freq = QInputDialog::getDouble(static_cast<_S*>(this),
                                          tr("设置工作频率"),
                                          tr("目标工频 (Hz):"),
                                          _workctrlTools._frequencySpin.value(),
                                          1,
                                          99,
                                          1,
                                          &ok);
      if (!ok)
        return;

      _workctrlTools._frequencySpin.setValue(freq);
      emit cmd_start(freq);
    });

    connect(&run._stop, &QAction::triggered, this, &_T::cmd_stop);

    connect(&run._pause, &QAction::triggered, this, &_T::cmd_pause);

    connect(&run._trigger, &QAction::triggered, this, &_T::cmd_trigger);

    auto& view = _menuBar._view;

    connect(&view._cascadeSubWindows,
            &QAction::triggered,
            &_mdiCenter,
            &QMdiArea::cascadeSubWindows);

    connect(&view._tileSubWindows,
            &QAction::triggered,
            &_mdiCenter,
            &QMdiArea::tileSubWindows);

    //    connect(&view._hideAll, &QAction::triggered, this, [this]() {
    //      for (auto&& i : _subUis) {
    //        i->
    //        i->set_conftime_visible(false);
    //        i->set_runtime_visible(false);
    //      }
    //    });

    auto& help = _menuBar._help;
    connect(&help._aboutEystack,
            &QAction::triggered,
            &_aboutEyestack,
            &QDialog::exec);
  }

  addToolBar(&_configurerTools);
  {
    // 设定对象名以供 saveState 使用
    _configurerTools.setObjectName("_configurerTools");

    _configurerTools.addAction(&_menuBar._config._wizard);
    _configurerTools.addAction(&_menuBar._config._reset);
    _configurerTools.addAction(&_menuBar._config._import);
    _configurerTools.addAction(&_menuBar._config._export);
  }

  addToolBar(&_workctrlTools);
  {
    _workctrlTools.setObjectName("_workctrlTools");
    _workctrlTools.addAction(&_menuBar._run._trigger);

    connect(
      &_workctrlTools._startButton, &QToolButton::clicked, this, [this]() {
        if (_runningState == RunningState::kStarted)
          emit cmd_stop();
        else
          emit cmd_start(_workctrlTools._frequencySpin.value());
      });

    connect(
      &_workctrlTools._pauseButton, &QToolButton::clicked, this, [this]() {
        if (_runningState == RunningState::kPaused)
          emit cmd_stop();
        else
          emit cmd_pause();
      });
  }

  addToolBar(Qt::LeftToolBarArea, &_subUiPanel);
  {
    _subUiPanel.setObjectName("_mdiPanel");
    _subUiPanel.setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    _subUiPanel.addAction(&_menuBar._view._cascadeSubWindows);
    _subUiPanel.addAction(&_menuBar._view._tileSubWindows);
    _subUiPanel.addSeparator();
  }

  setCentralWidget(&_mdiCenter);
  {
    _mdiCenter.setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _mdiCenter.setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _mdiCenter.setTabsClosable(true);
  }

  setStatusBar(&_statusBar);
  {
    connect(&_statusBar._taskctrlButton,
            &QToolButton::clicked,
            &_taskctrlWindow,
            &QWidget::setVisible);
    connect(&_statusBar._notifierButton,
            &QToolButton::clicked,
            &_notifierWindow,
            &QWidget::setVisible);
  }

  set_state_stopped();
}

void
MainWindow::register_sub_ui(SubUi& subUi)
{
  subUi.setup_ui(*this);
  _subUis.append(&subUi);
}

void
MainWindow::set_state_started()
{
  // 修改运行菜单项状态
  _menuBar._run._trigger.setEnabled(true);

  // 修改运行工具条状态
  _workctrlTools.switch_started();

  // 运行状态禁止修改配置
  _menuBar._config.allow_modification(false);

  _runningState = RunningState::kStarted;
}

void
MainWindow::set_state_paused()
{
  // 修改运行菜单项状态
  _menuBar._run._trigger.setEnabled(true);

  // 修改运行工具条状态
  _workctrlTools.switch_paused();

  // 运行状态禁止修改配置
  _menuBar._config.allow_modification(false);

  _runningState = RunningState::kPaused;
}

void
MainWindow::set_state_stopped()
{
  // 修改运行菜单项状态
  _menuBar._run._trigger.setEnabled(false);

  // 修改运行工具条状态
  _workctrlTools.switch_stopped();

  // 停止状态才允许修改配置
  _menuBar._config.allow_modification(true);

  _runningState = RunningState::kStopped;
}

void
MainWindow::register_config_wizard(QDialog& wizard)
{
  wizard.setWindowIcon(windowIcon());
  _configWizard = &wizard;
  _menuBar._config._wizard.setVisible(true);
}

void
MainWindow::reset_prefs() noexcept
{
  resize(sizeHint());

  for (auto&& i : _subUis)
    i->reset_prefs();
}

bool
MainWindow::dump_prefs(const QString& path) noexcept
{
  QFile prefFile(path);

  if (!prefFile.open(QFile::WriteOnly)) {
    qWarning(sLogcat) << "保存界面偏好失败：无法打开文件" << path;
    return false;
  }

  QDataStream ds(&prefFile);

  ds << saveGeometry();
  ds << saveState();

  for (auto&& i : _subUis) {
    if (!i->dump_prefs(ds)) {
      qWarning(sLogcat) << "保存界面偏好失败：无法保存子界面" << i;
      return false;
    }
  }

  qInfo(sLogcat) << "保存界面偏好成功。";
  return true;
}

bool
MainWindow::load_prefs(const QString& path) noexcept
{
  QFile prefFile(path);

  if (!prefFile.open(QFile::ReadOnly)) {
    qWarning(sLogcat) << "无法打开界面偏好文件，重置界面到默认值。" << path;
    reset_prefs();
    return false;
  }

  QDataStream ds(&prefFile);

  QByteArray ba;
  ds >> ba;
  restoreGeometry(ba);
  ds >> ba;
  restoreState(ba);

  for (auto&& i : _subUis) {
    if (!i->load_prefs(ds)) {
      qWarning(sLogcat) << "加载界面偏好失败，重置界面到默认值。";
      reset_prefs();
      return false;
    }
  }

  qInfo(sLogcat) << "加载界面偏好成功。";
  return true;
}

void
Eyestack::Framework::MainWindow::update_the_two_position()
{
  // 窗口的左下角对齐按钮的左上角
  auto gpos = _statusBar._taskctrlButton.mapToGlobal(QPoint(0, 0));
  _taskctrlWindow.move(gpos.x(), gpos.y() - _taskctrlWindow.height());

  // 窗口的右下角对齐按钮的右上角
  gpos = _statusBar._notifierButton.mapToGlobal(
    QPoint(_statusBar._notifierButton.width(), 0));
  _notifierWindow.move(gpos.x() - _notifierWindow.width(),
                       gpos.y() - _notifierWindow.height());
}

void
MainWindow::showEvent(QShowEvent* event)
{
  _S::showEvent(event);
  QTimer::singleShot(0, this, &_T::update_the_two_position);
}

void
MainWindow::hideEvent(QHideEvent* event)
{
  _S::hideEvent(event);
  _taskctrlWindow.hide();
  _notifierWindow.hide();
}

void
MainWindow::moveEvent(QMoveEvent* event)
{
  _S::moveEvent(event);
  update_the_two_position();
}

void
MainWindow::resizeEvent(QResizeEvent* event)
{
  _S::resizeEvent(event);
  update_the_two_position();
}

// ==========================================================================
// MainWindow::MenuBar
// ==========================================================================

MainWindow::MenuBar::MenuBar(QWidget* parent)
  : QMenuBar(parent)
{
  addMenu(&_view);
  addMenu(&_config);
  addMenu(&_run);
  addMenu(&_help);
}

MainWindow::MenuBar::Config::Config(QWidget* parent)
  : QMenu(tr("配置(&C)"), parent)
  , _wizard(faicon("hat-wizard-solid"), tr("向导(&W)..."))
  , _reset(faicon("history-solid"), tr("重置(&R)"))
  , _import(faicon("file-import-solid"), tr("导入(&I)..."))
  , _export(faicon("file-export-solid"), tr("导出(&E)..."))
{
  _wizard.setVisible(false);

  _wizard.setToolTip(tr("打开配置向导"));
  _reset.setToolTip(tr("重置为默认配置"));
  _import.setToolTip(tr("从文件导入配置"));
  _export.setToolTip(tr("导出配置到文件"));

  addAction(&_wizard);
  addAction(&_reset);
  addAction(&_import);
  addAction(&_export);
}

MainWindow::MenuBar::Run::Run(QWidget* parent)
  : QMenu(tr("运行(&R)"), parent)
  , _stop(faicon("stop-solid"), tr("停止(&S)"))
  , _start(faicon("play-solid"), tr("启动(&R)..."))
  , _pause(faicon("pause-solid"), tr("暂停(&P)"))
  , _trigger(faicon("wave-square-solid"), tr("触发(&T)"))
{
  _stop.setToolTip(tr("停止运行"));
  _start.setToolTip(tr("自动运行模式"));
  _pause.setToolTip(tr("待机运行模式"));
  _trigger.setToolTip(tr("触发一次工作"));

  addAction(&_stop);
  addAction(&_start);
  addAction(&_pause);
  addAction(&_trigger);
}

MainWindow::MenuBar::View::View(QWidget* parent)
  : QMenu(tr("视图(&V)"), parent)
  , _cascadeSubWindows(faicon("pager-solid"), tr("级联窗口"))
  , _tileSubWindows(faicon("table-cells-solid"), tr("平铺窗口"))
{
  addAction(&_cascadeSubWindows);
  addAction(&_tileSubWindows);

  addSeparator();
}

MainWindow::MenuBar::Help::Help(QWidget* parent)
  : QMenu(tr("帮助(&H)"), parent)
  , _aboutEystack(Asset::icon("eyestack"), tr("关于Eyestack..."))
{
  addAction(&_aboutEystack);
}

// ==========================================================================
// MainWindow::StatusBar
// ==========================================================================

MainWindow::StatusBar::StatusBar(QWidget* parent)
  : QStatusBar(parent)
  , _statusLabel(tr("就绪"))
  , _cycleLabel(tr("算法周期(ms):"))
  , _cycleLcd(4)
  , _frequencyLabel(tr("工频(Hz):"))
{
  // 布局状态栏
  addWidget(&_taskctrlButton);
  addWidget(&_statusLabel);
  addPermanentWidget(&_cycleLabel);
  addPermanentWidget(&_cycleLcd);
  addPermanentWidget(&_frequencyLabel);
  addPermanentWidget(&_frequencyLcd);
  addPermanentWidget(&_notifierButton);

  // 初始化组件状态
  _taskctrlButton.setIcon(faicon("tasks-solid"));
  _taskctrlButton.setCheckable(true);

  _cycleLcd.setSegmentStyle(QLCDNumber::Flat);
  _frequencyLcd.setSegmentStyle(QLCDNumber::Flat);

  _notifierButton.setIcon(faicon("bell-solid"));
  _notifierButton.setCheckable(true);
}

// ==========================================================================
// MainWindow::WorkctrlPanel
// ==========================================================================

MainWindow::WorkctrlTools::WorkctrlTools(QWidget* parent)
  : QToolBar(parent)
{
  setLayoutDirection(Qt::RightToLeft);

  // 在之后添加的子控件不受上面布局方向改变的影响
  addWidget(&_frequencySpin);
  addWidget(&_startButton);
  addWidget(&_pauseButton);

  _frequencySpin.setMinimum(1);
  _frequencySpin.setMaximum(99);
  _frequencySpin.setDecimals(1);
  _frequencySpin.setSuffix(" Hz");

  switch_stopped();
}

void
MainWindow::WorkctrlTools::switch_stopped()
{
  _frequencySpin.setEnabled(true);
  _startButton.setIcon(faicon("play-solid"));
  _pauseButton.setIcon(faicon("pause-solid"));
}

void
MainWindow::WorkctrlTools::switch_started()
{
  _frequencySpin.setEnabled(false);
  _startButton.setIcon(faicon("stop-solid"));
  _pauseButton.setIcon(faicon("pause-solid"));
}

void
MainWindow::WorkctrlTools::switch_paused()
{
  _frequencySpin.setEnabled(false);
  _startButton.setIcon(faicon("play-solid"));
  _pauseButton.setIcon(faicon("stop-solid"));
}

// ==========================================================================
// MainWindow::TaskctrlWindow
// ==========================================================================

Eyestack::Framework::MainWindow::TaskctrlWindow::TaskctrlWindow(QWidget* parent)
  : Design::StyleWindow(parent)
{
  // 通知框
  setWindowFlag(Qt::Tool);
  setMinimumSize(200, 150);
  setMaximumSize(600, 450);
  setBackgroundRole(QPalette::Mid);

  // 通知框标题栏
  {
    _titleBar.setLayout(&_titleLayout);
    _titleLayout.setContentsMargins(5, 3, 5, 3);
    _titleLayout.setSpacing(0);

    _titleLayout.addWidget(&_hideButton);
    _hideButton.setIcon(faicon("caret-down-solid"));

    _titleLayout.addWidget(&_titleLabel);
    _titleLabel.setText(tr("后台"));
    _titleLabel.setAlignment(Qt::AlignCenter);
    _titleLabel.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    _titleLayout.addWidget(&_sizeGrip);
    _sizeGrip.resize(_sizeGrip.width(),
                     _titleBar.height()); // 让 sizeGrip 垂直方向居中

    connect(&_hideButton, &QToolButton::clicked, this, &QWidget::hide);
  }

  // 通知框内容区
  {
    set_body(&_listScroll);
    _listScroll.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  }
}

void
Eyestack::Framework::MainWindow::TaskctrlWindow::showEvent(QShowEvent* event)
{
  auto mw = reinterpret_cast<MainWindow*>(parentWidget());
  mw->update_the_two_position();
  mw->_statusBar._taskctrlButton.setChecked(true);
  Design::StyleWindow::showEvent(event);
}

void
MainWindow::TaskctrlWindow::hideEvent(QHideEvent* event)
{
  Design::StyleWindow::hideEvent(event);
  auto mw = reinterpret_cast<MainWindow*>(parentWidget());
  mw->_statusBar._taskctrlButton.setChecked(false);
}

// ==========================================================================
// MainWindow::NotifierWindow
// ==========================================================================

MainWindow::NotifierWindow::NotifierWindow(QWidget* parent)
  : Design::StyleWindow(parent)
{
  // 通知框
  setWindowFlag(Qt::Tool); // 使窗口始终保持在其父窗口上，即使失去焦点
  setMinimumSize(200, 150);
  setMaximumSize(600, 450);
  setBackgroundRole(QPalette::Mid);

  // 通知框标题栏
  {
    _titleBar.setLayout(&_titleLayout);
    _titleLayout.setContentsMargins(5, 3, 5, 3);
    _titleLayout.setSpacing(0);

    _titleLayout.addWidget(&_sizeGrip);
    _sizeGrip.resize(_sizeGrip.width(),
                     _titleBar.height()); // 让 sizeGrip 垂直方向居中

    _titleLayout.addWidget(&_titleLabel);
    _titleLabel.setText(tr("通知"));
    _titleLabel.setAlignment(Qt::AlignCenter);
    _titleLabel.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    _titleLayout.addWidget(&_clearButton);
    _clearButton.setIcon(faicon("broom-solid"));

    _titleLayout.addWidget(&_hideButton);
    _hideButton.setIcon(faicon("caret-down-solid"));

    connect(&_hideButton, &QToolButton::clicked, this, &QWidget::hide);
    connect(&_clearButton, &QToolButton::clicked, this, [this]() {
      auto& layout = _listScroll._boardLayout;
      QLayoutItem* child;
      while ((child = layout.takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
      }
    });
  }

  // 通知框内容区
  {
    set_body(&_listScroll);
    _listScroll.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  }
}

void
Eyestack::Framework::MainWindow::NotifierWindow::showEvent(QShowEvent* event)
{
  auto mw = reinterpret_cast<MainWindow*>(parentWidget());
  mw->update_the_two_position();
  mw->_statusBar._notifierButton.setChecked(true);
  Design::StyleWindow::showEvent(event);
}

void
MainWindow::NotifierWindow::hideEvent(QHideEvent* event)
{
  Design::StyleWindow::hideEvent(event);
  auto mw = reinterpret_cast<MainWindow*>(parentWidget());
  mw->_statusBar._notifierButton.setChecked(false);
}

// ==========================================================================
// ↓ MainWindow::SubUi::MdiAction ↓
// ==========================================================================

MainWindow::SubUi::MdiAction::MdiAction(const QString& name, const QIcon& icon)
  : _action(icon, name)
{
  setWindowTitle(name);
  setWindowIcon(icon);
  _action.setCheckable(true);
  connect(&_action, &QAction::toggled, this, &_T::setVisible);
}

MainWindow::SubUi::MdiAction::~MdiAction()
{
  if (widget())
    widget()->setParent(nullptr);
}

void
MainWindow::SubUi::MdiAction::setup_ui(MainWindow& mw) noexcept
{
  mw._subUiPanel.addAction(&_action);
  mw._mdiCenter.addSubWindow(this);
}

void
MainWindow::SubUi::MdiAction::reset_prefs() noexcept
{
  adjustSize();
  hide();
}

bool
MainWindow::SubUi::MdiAction::dump_prefs(QDataStream& ds) noexcept
{
  ds << _action.isChecked();
  ds << saveGeometry();
  return true;
}

bool
MainWindow::SubUi::MdiAction::load_prefs(QDataStream& ds) noexcept
{
  bool visiable;
  ds >> visiable;
  setVisible(visiable);

  QByteArray data;
  ds >> data;
  return restoreGeometry(data);
}

void
MainWindow::SubUi::MdiAction::showEvent(QShowEvent* event)
{
  QMdiSubWindow::showEvent(event);

  // QMdiSubWindow 在关闭后重新显示，里面的控件不会自动被显示
  if (widget())
    widget()->show();

  if (!_action.isChecked())
    _action.setChecked(true);
}

void
MainWindow::SubUi::MdiAction::hideEvent(QHideEvent* event)
{
  QMdiSubWindow::hideEvent(event);
  if (_action.isChecked())
    _action.setChecked(false);
}

// ==========================================================================
// ↓ MainWindow::SubUi::MdiEx ↓
// ==========================================================================

MainWindow::SubUi::MdiMenu::MdiMenu(const QString& name, const QIcon& icon)
  : _S(name)
{
  setIcon(icon);
}

MainWindow::SubUi::MdiMenu::~MdiMenu()
{
  for (auto&& i : _mdiSubWindows)
    delete i;
}

MainWindow::SubUi::MdiAction&
MainWindow::SubUi::MdiMenu::add_mdi_action(const QString& name,
                                           QWidget& widget,
                                           QMenu& menu)
{
  auto mdi = new MdiAction(name);
  mdi->setWidget(&widget);

  menu.addAction(&mdi->_action);

  // MDI 界面尺寸必须异步更新
  QTimer::singleShot(0, mdi, [mdi]() { mdi->adjustSize(); });

  _mdiSubWindows.append(mdi);
  return *mdi;
}

void
MainWindow::SubUi::MdiMenu::setup_ui(MainWindow& mw) noexcept
{
  mw._subUiPanel.addAction(menuAction());
  for (auto&& i : _mdiSubWindows)
    mw._mdiCenter.addSubWindow(i);
}

void
MainWindow::SubUi::MdiMenu::reset_prefs() noexcept
{
  for (auto&& i : _mdiSubWindows)
    i->reset_prefs();
}

bool
MainWindow::SubUi::MdiMenu::dump_prefs(QDataStream& ds) noexcept
{
  for (auto&& i : _mdiSubWindows) {
    if (!i->dump_prefs(ds))
      return false;
  }
  return true;
}

bool
MainWindow::SubUi::MdiMenu::load_prefs(QDataStream& ds) noexcept
{
  for (auto&& i : _mdiSubWindows) {
    if (!i->load_prefs(ds))
      return false;
  }
  return true;
}

}

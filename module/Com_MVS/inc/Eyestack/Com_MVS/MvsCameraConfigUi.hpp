#pragma once

#include "Eyestack/Com_MVS/MvsCameraWrapper.hpp"
#include "Eyestack/Design/Monitor.hpp"
#include "ginit.hpp"

Q_DECLARE_METATYPE(::Eyestack::Com_MVS::MvsCameraWrapper::Shared);

namespace Eyestack::Com_MVS {

namespace Ui {
class MvsCameraConfigUi;
}

/**
 * @brief MVS 相机组件配置界面
 */
class EYESTACK_COM_MVS_EXPORT MvsCameraConfigUi
  : public QWidget
  , private GinitRequired
{
  Q_OBJECT

  using _T = MvsCameraConfigUi;
  using _S = QWidget;

public:
  MvsCameraConfigUi(QWidget* parent = nullptr);
  ~MvsCameraConfigUi();

public:
  /**
   * @brief 获取界面上的运行时监视器组件
   */
  Design::Monitor& running_monitor();

public:
  /**
   * @brief 获取当前选择的相机
   */
  MvsCameraWrapper::Shared chosen() { return _chosen; }

  bool colored();

  int timeout();

  bool monitored();

signals:
  void u_chosen(::Eyestack::Com_MVS::MvsCameraWrapper::Shared select);

  void u_colored(bool colored);

  void u_timeout(int timeout);

  void u_monitored(bool monitored);

public slots:
  /**
   * @brief 设置当前选择的相机，不发射更新信号。
   */
  void set_chosen(MvsCameraWrapper::Shared select)
  {
    _chosen = select;
    update_chosen();
  }

  void set_colored(bool colored);

  void set_timeout(int timeout);

  void set_monitored(bool monitored);

public slots:
  /**
   * @brief 设置当前选择的相机，发射更新信号。
   */
  void setChosen(MvsCameraWrapper::Shared select)
  {
    set_chosen(select);
    emit u_chosen(select);
  }

  void setColored(bool colored)
  {
    set_colored(colored);
    emit u_colored(colored);
  }

  void setTimeout(int timeout)
  {
    set_timeout(timeout);
    emit u_timeout(timeout);
  }

  void setMonitored(bool monitored)
  {
    set_monitored(monitored);
    emit u_monitored(monitored);
  }

private:
  struct Wrapper
  {
    MvsCameraWrapper::Shared _camera;
    bool _grabbing{ false };
  };

private:
  Ui::MvsCameraConfigUi* _ui;

  QVector<Wrapper> _wrappers;

  class Model : public QAbstractListModel
  {
  public:
    Model(MvsCameraConfigUi& self)
      : _self(self)
    {}

  private:
    MvsCameraConfigUi& _self;

    // QAbstractItemModel interface
  public:
    virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
    virtual int rowCount(const QModelIndex& parent) const override;
    virtual QVariant data(const QModelIndex& index, int role) const override;
    virtual bool setData(const QModelIndex& index,
                         const QVariant& value,
                         int role) override;
  } _model{ *this };

  MvsCameraWrapper::Shared _chosen;
  QModelIndex _selected;
  QTimer _timer;

  BOOL m_bOpenDevice;    // ch:是否打开设备 | en:Whether to open device
  BOOL m_bStartGrabbing; // ch:是否开始抓图 | en:Whether to start grabbing
  int m_nTriggerMode;    // ch:触发模式 | en:Trigger Mode
  int m_nTriggerSource;  // ch:触发源 | en:Trigger Source
  CRITICAL_SECTION m_hSaveImageMux;
  unsigned char* m_pSaveImageBuf;
  unsigned int m_nSaveImageBufSize;
  MV_FRAME_OUT_INFO_EX m_stImageInfo;
  double m_dExposureEdit;

private:
  // 计算并更新所有与 _chosen 有关的界面组件状态
  void update_chosen();

  // 计算并更新所有与 _selected 有关的界面组件状态
  void update_selected();

private slots:
  // 点击刷新按钮重建相机列表，也是重新初始化 _model
  void on_refreshButton_clicked();

  // 点击“应用选中”按钮设置组件使用的相机
  void on_chooseButton_clicked();

  // 单击列表中一项切换当前显示的相机配置
  void on_camlist_clicked(const QModelIndex& index);

  // 双击列表中一项切换相机的打开和取流状态
  void on_camlist_doubleClicked(const QModelIndex& index);

  // 配置界面的电源键控制监视定时器的启动和停止
  void when_configMonitor_powerClicked(bool power);

  // 当 _timer 计时结束时取帧刷新到界面上
  void when_timer_timeout();

  // 色彩模式选择
  void on_grayscaleRadio_clicked(bool checked);
  void on_coloredRadio_clicked(bool checked);

  // 相机属性按钮
  void on_impropButton_clicked();
  void on_expropButton_clicked();
};

}

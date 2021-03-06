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

  BOOL m_bOpenDevice;    // 是否打开设备
  BOOL m_bStartGrabbing; // 是否开始抓图
  int m_nTriggerMode;    // 触发模式
  int m_nTriggerSource;  // 触发源
  int m_nLineSelectorCombo;
  int m_nLinemodeCombo;
  MV_FRAME_OUT_INFO_EX m_stImageInfo;
  double m_dExposureEdit;
  BOOL m_bSoftWareTriggerCheck;

private:
  // 计算并更新所有与 _chosen 有关的界面组件状态
  void update_chosen();

  // 计算并更新所有与 _selected 有关的界面组件状态
  void update_selected();

  // 获取和设置触发模式
  void SetTriggerMode();
  void GetTriggerMode();

  // 获取和设置曝光时间
  void GetExposureTime();
  void SetExposureTime();

  // 获取和设置触发源
  void GetTriggerSource();
  void SetTriggerSource();

  // 设置触发使能
  void SetTriggerEnable();

  // 自定义格式不支持显示
  bool RemoveCustomPixelFormats(enum MvGvspPixelType enPixelFormat);

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

  // 连续模式按钮
  void on_ContinueButton_clicked();

  // 触发模式按钮
  void on_TriggerButton_clicked();

  // 软触发设置
  void on_SoftTriggerOnce_clicked();

  // 保存图片按钮
  void on_SaveBMPButton_clicked();

  // 设置曝光时间
  void on_SetExposureTime_clicked();

  // 获取曝光时间
  void on_GetExposureTime_clicked();

  // 保存PNG图像
  void on_SavePNGButton_clicked();

  // 线路选择设置按钮
  void on_m_ctrlGetLineselButton_clicked();

  // 线路选择获取按钮
  void on_m_ctrlSetLinesetButton_clicked();

  // 线路模式获取按钮
  void on_m_ctrlGetLinemodeButton_clicked();

  // 线路模式设置按钮
  void on_m_ctrlSetLinemodeButton_clicked();

  // 软触发按钮
  void on_SoftTrigger_stateChanged(int arg1);

  // 触发使能按钮
  void on_TriggerEnable_stateChanged(int arg1);
};
}

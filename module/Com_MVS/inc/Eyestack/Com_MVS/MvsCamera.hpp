#pragma once

#include "Eyestack/Com_MVS/MvsCameraWrapper.hpp"
#include "Eyestack/Framework/Workctrl.hpp"
#include "MvsCameraConfigUi.hpp"
#include "ginit.hpp"

Q_DECLARE_METATYPE(::Eyestack::Com_MVS::MvsError);

namespace Eyestack::Com_MVS {

class MvsCameraUi;

// ==========================================================================
// MvsCamera
// ==========================================================================

/**
 * @brief MVS 相机输入设备组件。
 */
class EYESTACK_COM_MVS_EXPORT MvsCamera final
  : public QObject
  , private GinitRequired
{
  Q_OBJECT

  using _T = MvsCamera;
  using _S = QObject;

public:
  class SubUi;

public:
  /**
   * @brief 构造函数
   * @param worker 所处的工作对象
   */
  MvsCamera(Framework::Worker& worker)
    : _S(&worker)
  {}

public:
  /**
   * @threadsafe
   * @brief 获取所处的工作对象
   */
  Framework::Worker& worker()
  {
    auto worker = dynamic_cast<Framework::Worker*>(_S::parent());
    Q_ASSERT(worker);
    return *worker;
  }

  /**
   * @brief 拍摄图像
   *
   * 注意 cv::Mat 内部采用了隐式共享，当监视开启时，返回对象会同时发送到
   * 连接的槽，所以，对 cv::Mat 的修改在调用者和所有的槽之间同时可见。
   *
   * @return 如果没有配置相机，返回空 Mat，否则返回 Mat 的格式取决于
   * 具体使用相机的色彩模式。
   *
   * @throw 可能抛出 MvsError 异常
   */
  cv::Mat snap() noexcept(false)
  {
    auto mat = _camera->snap_cvmat(_timeout, _colored);
    if (_monitored)
      emit s_refresh(mat);
    return mat;
  }

public:
  /**
   * @brief 当前使用的相机
   */
  MvsCameraWrapper::Shared camera() { return _camera; }

  /**
   * @brief 取帧超时时间
   */
  int timeout() { return _timeout; }

  /**
   * @brief 组件是否使用彩色图像
   */
  bool colored() { return _colored; }

  /**
   * @brief 组件是否启用监视
   */
  bool monitored() { return _monitored; }

signals:
  /**
   * @brief 当监视启用时，每次组件从数据源读取一帧图像都会发送此信号
   *
   * 注意 cv::Mat 内部采用了隐式共享，对 frame 的修改在所有槽之间可见！
   *
   * @param frame 获取的图像帧
   */
  void s_refresh(::cv::Mat frame);

  void u_camera(::Eyestack::Com_MVS::MvsCameraWrapper::Shared camera);

  void u_timeout(int msec);

  void u_colored(bool colored);

  void u_monitored(bool flag);

public slots:
  void set_camera(MvsCameraWrapper::Shared camera) { _camera = camera; }
  void setCamera(MvsCameraWrapper::Shared camera)
  {
    set_camera(camera);
    emit u_camera(camera);
  }

  void set_timeout(int timeout) { _timeout = timeout; }
  void setTimeout(int timeout)
  {
    set_timeout(timeout);
    emit u_timeout(timeout);
  }

  void set_colored(bool colored) { _colored = colored; }
  void setColored(bool colored)
  {
    set_colored(colored);
    emit u_colored(colored);
  }

  void set_monitored(bool flag) { _monitored = flag; }
  void setMonitored(bool flag)
  {
    set_monitored(flag);
    emit u_monitored(flag);
  }

private:
  MvsCameraWrapper::Shared _camera;
  int _timeout{ 100 };
  bool _colored{ false };
  bool _monitored{ false };
};

// ==========================================================================
// MvsCamera::SubUi
// ==========================================================================

class EYESTACK_COM_MVS_EXPORT MvsCamera::SubUi
  : public Framework::MainWindow::SubUi::MdiAction
{
  Q_OBJECT

  using _T = MvsCamera::SubUi;
  using _S = Framework::MainWindow::SubUi::MdiAction;

public:
  SubUi(MvsCamera& com,
        const QString& name,
        const QIcon& icon = Asset::faicon("camera-solid"));

private:
  MvsCameraConfigUi _configUi;
};

}

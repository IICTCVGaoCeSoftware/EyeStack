#include "Eyestack/Com_MVS/MvsCamera.hpp"

namespace Eyestack::Com_MVS {

using namespace Base::exc;
using namespace Base::util::serialize;

// ==========================================================================
// MvsCamera
// ==========================================================================

MvsCamera::SubUi::SubUi(MvsCamera& com, const QString& name, const QIcon& icon)
  : _S(name, icon)
{
  setWidget(&_configUi);

  connect(
    &_configUi, &MvsCameraConfigUi::u_chosen, &com, &MvsCamera::setCamera);
  connect(
    &com, &MvsCamera::u_camera, &_configUi, &MvsCameraConfigUi::set_chosen);

  connect(
    &_configUi, &MvsCameraConfigUi::u_timeout, &com, &MvsCamera::setTimeout);
  connect(
    &com, &MvsCamera::u_timeout, &_configUi, &MvsCameraConfigUi::set_timeout);

  connect(
    &_configUi, &MvsCameraConfigUi::u_colored, &com, &MvsCamera::setColored);
  connect(
    &com, &MvsCamera::u_colored, &_configUi, &MvsCameraConfigUi::set_colored);

  connect(&_configUi,
          &MvsCameraConfigUi::u_monitored,
          &com,
          &MvsCamera::setMonitored);
  connect(&com,
          &MvsCamera::u_monitored,
          &_configUi,
          &MvsCameraConfigUi::set_monitored);

  connect(&com,
          &MvsCamera::s_refresh,
          &_configUi.running_monitor(),
          &Design::Monitor::display_cvmat);

  // 初始化状态
  _configUi.set_chosen(com.camera());
  _configUi.set_timeout(com.timeout());
  _configUi.set_monitored(com.monitored());
  _configUi.set_colored(com.colored());
}

}

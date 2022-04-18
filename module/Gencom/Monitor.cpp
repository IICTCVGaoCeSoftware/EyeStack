#include "Eyestack/Gencom/Monitor.hpp"
#include "Eyestack/Asset.hpp"

using namespace Eyestack::Base::util;

namespace Eyestack::Gencom {

// ==========================================================================
// Monitor::SubUi
// ==========================================================================

Monitor::SubUi::SubUi(Monitor& com, const QString& name, const QIcon& icon)
  : _S(name, icon)
  , _com(com)
{
  setWidget(&_monitor);
  _monitor.set_caption(name);

  connect(&com,
          &Monitor::s_displayQImage,
          &_monitor,
          &Design::Monitor::display_qimage,
          Qt::QueuedConnection);
  connect(&com,
          &Monitor::s_displayCvMat,
          &_monitor,
          &Design::Monitor::display_cvmat,
          Qt::QueuedConnection);

  connect(&_monitor,
          &Design::Monitor::s_powerClicked,
          &com,
          &Monitor::setMonitored,
          Qt::QueuedConnection);
  connect(&com,
          &Monitor::s_monitoredUpdated,
          &_monitor,
          &Design::Monitor::set_power,
          Qt::QueuedConnection);
}

}

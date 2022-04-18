#include "Eyestack/Algocom/Paramgr.hpp"

namespace Eyestack::Algocom {

ParamgrUi*
Paramgr::new_ui()
{
  return new ParamgrUi(*this);
}

void
Paramgr::reset_config()
{
  _paramTable.reset();
}

void
Paramgr::dump_config(QDataStream& ds)
{
  _paramTable.dump(ds);
}

void
Paramgr::load_config(QDataStream& ds)
{
  _paramTable.load(ds);
}

ParamgrUi::ParamgrUi(Paramgr& com)
{
  _configUi.set_table(com._paramTable);

  QObject::connect(
    &com, &Paramgr::monitoredUpdated, &_runningUi, &Design::Monitor::setPower);
  QObject::connect(
    &_runningUi, &Design::Monitor::powerClicked, &com, &Paramgr::setMonitored);

  QObject::connect(
    &com, &Paramgr::refreshMonitor, &_runningUi, &Design::Monitor::setScreen);
}

}

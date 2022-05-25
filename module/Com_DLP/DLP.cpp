#include "Eyestack/Com_DLP/DLP.hpp"

namespace Eyestack::Com_DLP {

// ==========================================================================
// DLP
// ==========================================================================

DLP::SubUi::SubUi(DLP& com, const QString& name, const QIcon& icon)
  : _S(name, icon)
{
  setWidget(&_configUi);
}

}

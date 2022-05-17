#include "Eyestack/Com_DLP/ginit.hpp"

namespace Eyestack::Com_DLP {

#ifdef _DEBUG
static bool sGinited = false;
#endif

void
ginit()
{  
#ifdef _DEBUG
  Q_ASSERT(sGinited == false);
  sGinited = true;
#endif

  qRegisterMetaType<::cv::Mat>("::cv::Mat");
}

#ifdef _DEBUG
void
ginit_required()
{
  Q_ASSERT(sGinited);
}

GinitRequired::GinitRequired()
{
  Q_ASSERT(sGinited);
}
#endif

}

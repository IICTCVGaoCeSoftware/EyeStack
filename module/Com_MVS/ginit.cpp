#include "Eyestack/Com_MVS/ginit.hpp"
#include "Eyestack/Com_MVS/MvsCamera.hpp"

namespace Eyestack::Com_MVS {

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
  qRegisterMetaType<MvsError>();
  qRegisterMetaType<MvsCameraWrapper>();
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

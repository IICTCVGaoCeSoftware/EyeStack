#include "Eyestack/Gencom.hpp"

namespace Eyestack::Gencom {

static bool sGinited = false;

void
ginit()
{
  Q_ASSERT(sGinited == false);
  sGinited = true;

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

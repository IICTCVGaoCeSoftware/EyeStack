#include "Eyestack/Devcom/ImageBench.hpp"

namespace Eyestack::Devcom {

ImageBenchUi*
ImageBench::new_ui()
{
  return new ImageBenchUi(*this);
}

void
ImageBenchUi::switch_conftime()
{
  _runningUi.disconnect();
  QObject::connect(&_com,
                   &ImageBench::sigDisplay,
                   &_configUi,
                   &Design::ImageBench::displayQImage);
}

void
ImageBenchUi::switch_runtime()
{
  _configUi.disconnect();
  QObject::connect(&_com,
                   &ImageBench::sigDisplay,
                   &_runningUi,
                   &Design::ImageBench::displayQImage);
}

}

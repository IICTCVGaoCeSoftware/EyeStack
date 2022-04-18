/**
 * @file Sobel.cpp
 */

#include "algo/Sobel.hpp"
#include "Eyestack/Base.hpp"

namespace Getting_Started {

Sobel::Sobel()
{
  _paramTable.register_param(_ddepth);
  _paramTable.register_param(_dx);
  _paramTable.register_param(_dy);
  _paramTable.register_param(_ksize);
  _paramTable.register_param(_scale);
  _paramTable.register_param(_delta);

  _paramTable.register_param(_enableLogger);
}

cv::Mat
Sobel::process(const cv::Mat& src)
{
  namespace esb = Eyestack::Base;
  esb::Logger logger("Getting_Started/");

  if (!_enableLogger)
    logger.disable();

  logger.image("src.png") << src;

  cv::Mat dst;

  esb::Profiler prof;
  {
    esb::Profiler::Scoper scoper(prof, "cv::Sobel");

    cv::Sobel(src, dst, _ddepth, _dx, _dy, _ksize, _scale, _delta, _borderType);
  }
  logger.profiler("prof") << prof;

  logger.image("dst.png") << dst;

  return dst;
}

}

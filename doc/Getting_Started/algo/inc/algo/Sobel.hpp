/**
 * @file Sobel.hpp
 */

#pragma once

#include <Eyestack/Design/Paramgr.hpp>

namespace Getting_Started {

namespace esd = Eyestack::Design;
using namespace esd::paramgr;

/**
 * @brief Sobel 图像梯度算子，包装自 OpenCV 同名函数。
 */
class Sobel
{
public:
  Sobel();

public:
  /**
   * @brief 处理图片
   * @param src 输入图像
   * @return 输出图像
   */
  cv::Mat process(const cv::Mat& src);

  /**
   * @brief 获取 borderType 参数
   */
  int get_border_type() { return _borderType; }

  /**
   * @brief 设置 borderType 参数
   */
  void set_border_type(int val) { _borderType = val; }

  /**
   * @brief 获取算法的参数表
   */
  esd::Paramgr::Table& get_param_table() { return _paramTable; }

private:
  esd::Paramgr::Table _paramTable;

  int _borderType;

  Int _ddepth{ "ddepth", "输出图像深度", CV_8U };
  Int _dx{ "dx", "X 方向导数阶", 1 };
  Int _dy{ "dy", "Y 方向倒数阶", 1 };
  Int _ksize{ "ksize", "算子核大小", 3 };

  Double _scale{ "scale", "缩放因子", 1 };
  Double _delta{ "delta", "偏移值", 0 };

  Bool _enableLogger{ "enableLogger", "是否启用日志输出", true };
};

}

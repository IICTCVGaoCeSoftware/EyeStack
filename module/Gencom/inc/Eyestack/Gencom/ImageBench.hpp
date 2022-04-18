#pragma once

#include "Eyestack/Base.hpp"
#include "Eyestack/Design/ImageBench.hpp"

namespace Eyestack::Devcom {

class ImageBenchUi;

/**
 * @brief 一个简单的图像显示器组件，除此之外再无其它
 */
class EYESTACK_DEVCOM_EXPORT ImageBench
  : public QObject
  , public Base::com::Component::Default
{
  Q_OBJECT

public:
  using Color = Design::ImageBench::Color;

signals:
  void sigDisplay(QImage img);

public:
  /**
   * @brief 显示 QPixmap 图像
   */
  void display(const QPixmap& img) { emit sigDisplay(img.toImage()); }

  /**
   * @brief 显示 QImage 图像，这个方法保证会进行 QImage 的深拷贝。
   */
  void display(const QImage& img) { emit sigDisplay(img); }

  /**
   * @brief 显示 QImage 图像，这个方法会尽可能避免拷贝 QImage，小心指针悬挂。
   */
  void display(QImage&& img) { emit sigDisplay(std::move(img)); }

  /**
   * @brief 兼容 OpenCV 的图像显示，同时支持深浅拷贝
   */
  template<typename Mat>
  void display(Mat&& img, Color color)
  {
    emit sigDisplay(Base::util::cvmat_to_qimage(img), color);
  }

  /**
   * @brief 兼容 OpenCV 的图像显示，自动识别格式
   */
  template<typename Mat>
  void display(Mat&& img)
  {
    emit sigDisplay(Base::util::cvmat_to_qimage(img));
  }

  ImageBenchUi* new_ui();

  // Component interface
public:
  virtual uint64_t type_uuid() override { return 0xe8ba0425719ea25b; }
};

class EYESTACK_DEVCOM_EXPORT ImageBenchUi : public Base::com::MdiSubUi
{
public:
  ImageBenchUi(ImageBench& com)
    : _com(com)
  {}

private:
  ImageBench& _com;
  Design::ImageBench _configUi;
  Design::ImageBench _runningUi; // TODO 添加 GRAB 按钮

  // MdiSubUi interface
public:
  virtual QWidget* config_ui() override { return &_configUi; }
  virtual void switch_conftime() override;
  virtual QWidget* running_ui() override { return &_runningUi; }
  virtual void switch_runtime() override;
};

}

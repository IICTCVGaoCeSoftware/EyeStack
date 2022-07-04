#pragma once

#include "Eyestack/Base.hpp"

namespace Eyestack::Design {

// ==========================================================================
// ImageBenchBase
// ==========================================================================

/**
 * @brief ImageBench 的基类，仅包含界面描述的代码
 */
class EYESTACK_DESIGN_EXPORT ImageBenchBase : public QWidget
{
  Q_OBJECT

  using _T = ImageBenchBase;
  using _S = QWidget;

public:
  /**
   * @brief 自定义视图类，提供了拖动、缩放功能
   */
  class View : public QGraphicsView
  {
    using _T = View;
    using _S = QGraphicsView;

  public:
    View(ImageBenchBase& self);

  private:
    ImageBenchBase& _self;
    qreal _scale;

    // 用于拖拽与缩放功能
    bool _dragging;
    QPointF _mouseOrigin; // 拖拽开始时鼠标在 Scene 中的位置

    // 用于小地图功能
    QToolButton _mapTool;

  private:
    void scale(qreal factor);

    // QWidget interface
  protected:
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;
  };

  /**
   * @brief 自定义场景类，添加了绘制坐标轴的功能
   */
  class Scene : public QGraphicsScene
  {
    using _T = Scene;
    using _S = QGraphicsScene;

  public:
    using _S::_S;

    // QGraphicsScene interface
  protected:
    virtual void drawBackground(QPainter* painter, const QRectF& rect) override;
  };

  /**
   * @brief 自定义图像物体类，封装了 QPainter，并且提供超出图像边界的拖动功能。
   */
  class ImageItem : public QGraphicsItem
  {
    using _T = ImageItem;
    using _S = QGraphicsItem;

  public:
    /**
     * 使用父类的默认构造函数
     */
    using QGraphicsItem::QGraphicsItem;

  public:
    /**
     * @brief 获取当前的图像
     * @return 当前图像的引用，注意对图像的任何修改需要再调用 update 方法才能
     * 同步到界面上。
     */
    QImage& get_image() { return _image; }

    /**
     * @brief 设置图像（完美转发）
     */
    template<typename QImage>
    void set_image(QImage&& image)
    {
      if (_image.size() != image.size())
        prepareGeometryChange();
      _image = image;
      update(_image.rect());
    }

  protected:
    QImage _image;

    // QGraphicsItem interface
  public:
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter* painter,
                       const QStyleOptionGraphicsItem* option,
                       QWidget* widget) override;
    virtual QPainterPath shape() const override;
  };

public:
  ImageBenchBase(QWidget* parent = nullptr)
    : _S(parent)
    , _view(*this)
  {
    setup_ui();
  }

public:
  /**
   * @brief 显示 QPixmap 图像
   */
  void display(const QPixmap& img)
  {
    update_info_from_qpixmap(img);
    set_image(img.toImage());
  }

  /**
   * @brief 显示 QImage 图像，这个方法保证会进行 QImage 的深拷贝。
   */
  void display(const QImage& img)
  {
    update_info_from_qimage(img);
    set_image(img);
  }

  /**
   * @brief 显示 QImage 图像，这个方法会尽可能避免拷贝 QImage，小心指针悬挂。
   */
  void display(QImage&& img)
  {
    update_info_from_qimage(img);
    set_image(img);
  }

  /**
   * @brief 自动识别 OpenCV 的图像格式并显示，深拷贝版本
   */
  void display(const cv::Mat& img)
  {
    update_info_from_cvmat(img);
    set_image(Base::util::cvmat_to_qimage(img));
  }

  /**
   * @brief 自动识别 OpenCV 的图像格式并显示，浅拷贝版本
   */
  void display(cv::Mat&& img)
  {
    update_info_from_cvmat(img);
    set_image(Base::util::cvmat_to_qimage(img));
  }

  /**
   * @brief 获取当前正在显示的图像，返回只读引用
   */
  const QImage& current() { return get_image(); }

public slots:
  /**
   * @brief 方便利用 Qt 信号机制的槽，使用 QImage 值传递更安全
   */
  void display_qimage(QImage img) { display(std::move(img)); }

private:
  QLabel _scaleLabel;
  QLabel _mouseLabel;
  QLabel _infoLabel;

private:
  void setup_ui();
  void update_info_from_qpixmap(const QPixmap& img);
  void update_info_from_qimage(const QImage& img);
  void update_info_from_cvmat(const cv::Mat& mat);

protected:
  View _view; // 必须声明在 _xxxLabel 后
  // NOTE 继承这个类二次开发时，别忘了绑定 _view 到 scene 上！

protected:
  /**
   * @brief 获取当前图像
   * @return 当前图像的常引用
   */
  virtual const QImage& get_image() = 0;

  /**
   * @brief 设置图像（复制语义）
   * @param img 新图像
   */
  virtual void set_image(const QImage& img) = 0;

  /**
   * @brief 设置图像（移动语义）
   * @param img 新图像
   */
  virtual void set_image(QImage&& img) = 0;
};

// ==========================================================================
// ImageBench
// ==========================================================================

/**
 * @brief ImageBench 基于 QGraphicsView 实现，被设计用于增强功能的图像查看器
 * 和图像编辑器的二次开发。即使直接使用时，也已经提供了拖拽、缩放、取色、图像信息显示、
 * 场景坐标系绘制等增强功能。
 * 唯一的缺点是：ImageBench 在更换图像时的开销较大，因而其只适用于静态图片（而非
 * 视频）的编辑任务。
 */
class EYESTACK_DESIGN_EXPORT ImageBench : public ImageBenchBase
{
  using _T = ImageBench;
  using _S = ImageBenchBase;

public:
  ImageBench(QWidget* parent = nullptr);
  ~ImageBench();

protected:
  Scene _scene;
  ImageItem _imageItem; // 必须声明在 _scene 之后，否则会重复析构
  ImageItem _imageItem1;

  // ImageBenchBase interface
protected:
  virtual const QImage& get_image() override;
  virtual void set_image(const QImage& img) override;
  virtual void set_image(QImage&& img) override;
};

}

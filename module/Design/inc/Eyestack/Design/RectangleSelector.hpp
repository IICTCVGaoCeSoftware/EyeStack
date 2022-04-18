#include "Eyestack/Design/ImageBench.hpp"

namespace Eyestack::Design {

/**
 * @brief 四边形选择器，提供四个允许拖动的图钉，允许用户通过拖动图钉
 * 在图像上选取任意的四边形。
 */
class EYESTACK_DESIGN_EXPORT RectangleSelector : public ImageBenchBase
{
  using _T = RectangleSelector;
  using _S = ImageBenchBase;

public:
  /**
   * @param scattered 是否启用离散选点效果
   * @param parent 父组件指针
   */
  RectangleSelector(bool scattered = true, QWidget* parent = nullptr);

public:
  /**
   * @brief 获取离散选点启用状态
   * @return 启用返回 true，否则返回 false
   */
  bool get_scattered() { return _imageItem.get_scattered(); }

  /**
   * @brief 设置离散选点是否启用
   * @param 启用返回 true，否则返回 false
   */
  void set_scattered(bool scattered) { _imageItem.set_scattered(scattered); }

  /**
   * @brief 重设选择到整个图像
   */
  void reset_selection() { _imageItem.reset_selection(); }

  /**
   * @brief 获取当前选择的矩形区域（Qt格式）
   * @return QRectF 类型
   */
  QRectF get_qrect() { return _imageItem.get_rect(); }

  /**
   * @brief 设置选择的矩形区域（Qt格式）
   * @param rect 区域，QRectF 类型
   */
  void set_qrect(QRectF rect) { _imageItem.set_rect(rect); }

  /**
   * @brief 获取当前选择的矩形区域（OpenCV格式）
   * @return cv::Rect2f 类型
   */
  cv::Rect2f get_cv_rect()
  {
    auto rect = _imageItem.get_rect();
    return cv::Rect2f(rect.left(), rect.top(), rect.width(), rect.height());
  }

  /**
   * @brief 设置选择的矩形区域（OpenCV格式）
   * @param rect 区域，cv::Rect2f 类型
   */
  void set_cv_rect(cv::Rect2f rect)
  {
    _imageItem.set_rect({ rect.x, rect.y, rect.width, rect.height });
  }

private:
  class ImageItem : public _S::ImageItem
  {
    using _T = ImageItem;
    using _S = ImageBenchBase::ImageItem;

  public:
    ImageItem(bool scattered, QGraphicsItem* parent = nullptr)
      : _S(parent)
      , _scattered(scattered)
      , _rect()
      , _selecting(false)
      , _startPos()
    {}

  public:
    bool get_scattered() { return _scattered; }

    void set_scattered(bool scattered) { _scattered = scattered; }

    void reset_selection()
    {
      _rect = _image.rect();
      update(_rect);
    }

    QRectF get_rect() { return _rect; }

    void set_rect(QRectF rect)
    {
      auto area = _rect.united(rect);
      _rect = rect;
      update(rect.united(area));
    }

  private:
    bool _scattered;
    QRectF _rect;

    bool _selecting;
    QPointF _startPos;

    // QGraphicsItem interface
  public:
    virtual void paint(QPainter* painter,
                       const QStyleOptionGraphicsItem* option,
                       QWidget* widget) override;

  protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  };

  // TODO 添加矩形拖动效果

private:
  _S::Scene _scene;
  ImageItem _imageItem;

  // ImageBenchBase interface
protected:
  virtual const QImage& get_image() override;
  virtual void set_image(const QImage& img) override;
  virtual void set_image(QImage&& img) override;
};

}

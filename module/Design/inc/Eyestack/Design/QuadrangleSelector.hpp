#pragma once

#include "Eyestack/Design/ImageBench.hpp"

namespace Eyestack::Design {

/**
 * @brief 四边形选择器，提供四个允许拖动的图钉，允许用户通过拖动图钉
 * 在图像上选取任意的四边形。
 */
class EYESTACK_DESIGN_EXPORT QuadrangleSelector : public ImageBench
{
  using _T = QuadrangleSelector;
  using _S = ImageBench;

public:
  /**
   * @param scattered 是否启用离散选点效果
   * @param parent 父组件指针
   */
  QuadrangleSelector(bool scattered = true, QWidget* parent = nullptr);

public:
  enum struct Corner
  {
    kTopLeft = 0,
    kTopRight = 1,
    kBottomLeft = 2,
    kBottomRight = 3,
  };

public:
  /**
   * @brief 重设图钉到图像四角
   */
  void reset_thumbtacks();

  /**
   * @brief 获取图钉位置
   * @param cor 哪个图钉，枚举量
   * @return 图钉在图像坐标系中的位置
   */
  QPointF get_thumbtack(Corner cor) { return _thumbtacks[int(cor)].pos(); }

  /**
   * @brief 设置图钉位置
   * @param cor 哪个图钉，枚举量
   * @param pos 图钉在图像坐标系中的位置
   */
  void set_thumbtack(Corner cor, QPointF pos)
  {
    _thumbtacks[int(cor)].setPos(pos);
  }

  /**
   * @brief 获取离散选点启用状态
   * @return 启用返回 true，否则返回 false
   */
  bool get_scattered() { return _scattered; }

  /**
   * @brief 设置离散选点是否启用
   * @param 启用返回 true，否则返回 false
   */
  void set_scattered(bool scattered) { _scattered = scattered; }

  /**
   * @brief 以 OpenCV 格式获取当前的四边形
   * @return cv::Point2f 组成的 4 个元素长度的 cv::Vec 向量，顺序：↖↗↙↘
   */
  cv::Vec<cv::Point2f, 4> get_quadrangle();

  /**
   * @brief 以 OpenCV 格式设置当前四边形
   * @param points 至少 4 行 2 列的 CV_32FC1 类型数组，顺序：↖↗↙↘
   */
  void set_quadrangle(cv::InputArray points);

private:
  /**
   * @brief 用于拖动选取区域的图钉类
   */
  class ThumbtackItem : public QGraphicsItem
  {
    using _T = ThumbtackItem;
    using _S = QGraphicsItem;

  public:
    /**
     * @param x 三角形横边
     * @param y 三角形纵边
     * @param color 三角形颜色
     */
    ThumbtackItem(QuadrangleSelector& self,
                  int x,
                  int y,
                  QColor color,
                  QGraphicsItem* parent);

  private:
    int _x, _y;
    QColor _color;

  private:
    QuadrangleSelector& _self;
    QPointF _delta; // 用于实现拖动的颗粒效果

    // QGraphicsItem interface
  public:
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter* painter,
                       const QStyleOptionGraphicsItem* option,
                       QWidget* widget) override;

  protected:
    virtual QVariant itemChange(GraphicsItemChange change,
                                const QVariant& value) override;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  };

private:
  QGraphicsPathItem _borderItem; // 边框线
  QGraphicsPathItem _outBorderItem; // 边框线
  ThumbtackItem _thumbtacks[4];     // 顺序：↖↗↙↘
  ThumbtackItem _outThumbtacks[4];

  bool _scattered;

private:
  void update_lines();
};

}

#include "Eyestack/Design/QuadrangleSelector.hpp"

namespace Eyestack::Design {

// ==========================================================================
// QuadrangleSelector
// ==========================================================================

QuadrangleSelector::QuadrangleSelector(bool scattered, QWidget* parent)
  : _S(parent)
  , _borderItem(&_imageItem)
  , _outBorderItem(&_imageItem)
  , _thumbtacks{ { *this, -20, -20, Qt::red, &_imageItem },
                 { *this, 20, -20, Qt::green, &_imageItem },
                 { *this, -20, 20, Qt::blue, &_imageItem },
                 { *this, 20, 20, Qt::yellow, &_imageItem } }
  , _outThumbtacks{ { *this, -20, -20, Qt::red, &_imageItem },
                    { *this, 20, -20, Qt::green, &_imageItem },
                    { *this, -20, 20, Qt::blue, &_imageItem },
                    { *this, 20, 20, Qt::yellow, &_imageItem } }
  , _scattered(scattered)
{
  _borderItem.setPen(QPen(Qt::cyan));
}

void
QuadrangleSelector::reset_thumbtacks()
{
  auto rect = _imageItem.get_image().rect();
  _thumbtacks[0].setPos(rect.topLeft());
  _thumbtacks[1].setPos(rect.bottomLeft());
  _thumbtacks[2].setPos(rect.bottomRight());
  _thumbtacks[3].setPos(rect.topRight());
}

cv::Vec<cv::Point2f, 4>
QuadrangleSelector::get_quadrangle()
{
  cv::Vec<cv::Point2f, 4> retval;
  for (auto i = 0; i < 4; ++i) {
    auto point = _thumbtacks[i].pos();
    retval[i].x = point.x();
    retval[i].y = point.y();
  }
  return retval;
}

void
QuadrangleSelector::set_quadrangle(cv::InputArray points)
{
  auto mat = points.getMat();
  for (auto i = 0; i < 4; ++i) {
    QPointF qpoint(mat.at<float>(i, 0), mat.at<float>(i, 1));
    _thumbtacks[i].setPos(qpoint);
  }
}

void
QuadrangleSelector::update_lines()
{
  QPainterPath path;
  path.moveTo(_thumbtacks[0].pos());
  path.lineTo(_thumbtacks[1].pos());
  path.lineTo(_thumbtacks[3].pos());
  path.lineTo(_thumbtacks[2].pos());
  path.closeSubpath();
  _borderItem.setPath(std::move(path));

  QPainterPath path1;
  path1.moveTo(_outThumbtacks[0].pos());
  path1.lineTo(_outThumbtacks[1].pos());
  path1.lineTo(_outThumbtacks[3].pos());
  path1.lineTo(_outThumbtacks[2].pos());
  path1.closeSubpath();
  _outBorderItem.setPath(std::move(path1));
}

// ==========================================================================
// QuadrangleSelector::ThumbtackItem
// ==========================================================================

QuadrangleSelector::ThumbtackItem::ThumbtackItem(QuadrangleSelector& self,
                                                 int x,
                                                 int y,
                                                 QColor color,
                                                 QGraphicsItem* parent)
  : _S(parent)
  , _x(x)
  , _y(y)
  , _color(color)
  , _self(self)
{
  setFlags(QGraphicsItem::ItemIgnoresTransformations |
           QGraphicsItem::ItemSendsGeometryChanges);
}

QRectF
QuadrangleSelector::ThumbtackItem::boundingRect() const
{
  QRectF rect;

  if (_x < 0)
    rect.setLeft(_x), rect.setWidth(-_x);
  else
    rect.setLeft(0), rect.setWidth(_x);

  if (_y < 0)
    rect.setTop(_y), rect.setHeight(-_y);
  else
    rect.setTop(0), rect.setHeight(_y);

  return rect;
}

void
QuadrangleSelector::ThumbtackItem::paint(QPainter* painter,
                                       const QStyleOptionGraphicsItem* option,
                                       QWidget* widget)
{
  painter->setBrush(_color);
  painter->drawPolygon(QPolygon({ { 0, 0 }, { _x, 0 }, { 0, _y } }));
}

QVariant
QuadrangleSelector::ThumbtackItem::itemChange(GraphicsItemChange change,
                                              const QVariant& value)
{
  if (change == QGraphicsItem::ItemPositionHasChanged)
    _self.update_lines();
  return value;
}

void
QuadrangleSelector::ThumbtackItem::mousePressEvent(
  QGraphicsSceneMouseEvent* event)
{
  if (event->button() == Qt::LeftButton)
    _delta = scenePos() - event->scenePos();
}

void
QuadrangleSelector::ThumbtackItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  if (event->buttons() & Qt::LeftButton) {
    auto newPos = mapToParent(mapFromScene(event->scenePos() + _delta));
    if (_self._scattered) {
      newPos.setX(std::roundf(newPos.x()));
      newPos.setY(std::roundf(newPos.y()));
    }
    setPos(newPos);
  }
}

}

#include "Eyestack/Design/RectangleSelector.hpp"

namespace Eyestack::Design {

// ==========================================================================
// RectangleSelector
// ==========================================================================

RectangleSelector::RectangleSelector(bool scattered, QWidget* parent)
  : _S(parent)
  , _imageItem(scattered)
{
  _scene.addItem(&_imageItem);
  _view.setScene(&_scene);
}

const QImage&
RectangleSelector::get_image()
{
  return _imageItem.get_image();
}

void
RectangleSelector::set_image(const QImage& img)
{
  _imageItem.set_image(img);
}

void
RectangleSelector::set_image(QImage&& img)
{
  _imageItem.set_image(std::move(img));
}

// ==========================================================================
// RectangleSelector::ImageItem
// ==========================================================================

void
RectangleSelector::ImageItem::paint(QPainter* painter,
                                    const QStyleOptionGraphicsItem* option,
                                    QWidget* widget)
{
  _S::paint(painter, option, widget);

  painter->setPen(Qt::NoPen);
  auto color = option->palette.color(QPalette::Highlight);
  if (_selecting)
    color.setAlpha(127);
  else
    color.setAlpha(63);
  painter->setBrush(color);

  painter->drawRect(_rect);
}

void
RectangleSelector::ImageItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  if (event->button() == Qt::LeftButton) {
    _selecting = true;

    _startPos = event->pos();
    if (_scattered) {
      _startPos.setX(qRound(_startPos.x()));
      _startPos.setY(qRound(_startPos.y()));
    }

    update(_image.rect());

    // 必须在这里 accept 事件才能收到接下来的 move、release 等消息
    event->accept();
    return;
  }

  _S::mousePressEvent(event);
}

void
RectangleSelector::ImageItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  if (_selecting) {
    auto pos = event->pos();
    if (_scattered) {
      pos.setX(qRound(pos.x()));
      pos.setY(qRound(pos.y()));
    }

    QRectF rect;
    if (pos.x() < _startPos.x()) {
      if (pos.y() < _startPos.y()) {
        rect.setTopLeft(pos);
        rect.setBottomRight(_startPos);
      } else {
        rect.setBottomLeft(pos);
        rect.setTopRight(_startPos);
      }
    } else {
      if (pos.y() < _startPos.y()) {
        rect.setTopRight(pos);
        rect.setBottomLeft(_startPos);
      } else {
        rect.setBottomRight(pos);
        rect.setTopLeft(_startPos);
      }
    }
    rect = rect.intersected(_image.rect());

    update(_rect.united(rect));
    _rect = rect;
  }

  _S::mouseMoveEvent(event);
}

void
RectangleSelector::ImageItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  if (event->button() == Qt::LeftButton) {
    _selecting = false;
    update(_image.rect());
  }

  _S::mouseReleaseEvent(event);
}

}

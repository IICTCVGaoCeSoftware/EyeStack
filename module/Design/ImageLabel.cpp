#include "Eyestack/Design/ImageLabel.hpp"

namespace Eyestack::Design {

/**
 * @brief 将 objRect 保持宽高比缩放到 viewRect 并居中。
 */
static void
centered_view_rect(QRect& viewRect, const QRect& objRect)
{
  if (objRect.isEmpty()) {
    viewRect = QRect();
    return;
  }

  if (viewRect.width() * objRect.height() >
      objRect.width() * viewRect.height()) {
    auto newWidth = objRect.width() * viewRect.height() / objRect.height();
    viewRect.translate((viewRect.width() - newWidth) >> 1, 0);
    viewRect.setWidth(newWidth);

  } else {
    auto newHeight = objRect.height() * viewRect.width() / objRect.width();
    viewRect.translate(0, (viewRect.height() - newHeight) >> 1);
    viewRect.setHeight(newHeight);
  }
}

void
ImageLabel::paintEvent(QPaintEvent* event)
{
  _S::paintEvent(event);

  auto viewRect = rect();
  QPainter painter(this);

  if (_pixmap.isNull()) {
    painter.setPen(palette().color(QPalette::WindowText));

    // 先按 13 点大小试这绘制一下，再根据所得文本框相对视口框
    // 的比例放大点大小，重新绘制一个大小合适的。

    auto f = font();
    f.setPointSize(13);
    painter.setFont(f);

    QRect fullRect;
    painter.drawText(fullRect, Qt::AlignCenter | _flag, _text, &fullRect);

    centered_view_rect(viewRect, fullRect);

    if (!viewRect.isNull()) {
      f.setPointSize(0.6 * f.pointSize() * viewRect.height() /
                     fullRect.height());
      painter.setFont(f);

      painter.drawText(viewRect, Qt::AlignCenter | _flag, _text, &fullRect);
    }

    return;
  }

  centered_view_rect(viewRect, _pixmap.rect());
  painter.drawPixmap(viewRect, _pixmap, _pixmap.rect());
}

QSize
ImageLabel::sizeHint() const
{
  if (_pixmap.isNull() || _pixmap.width() < 400 || _pixmap.height() < 300) {
    return QSize(400, 300);
  }
  return _pixmap.size();
}

}

#include "Eyestack/Design/ListScroll.hpp"

namespace Eyestack::Design {

void
ListScroll::resizeEvent(QResizeEvent* event)
{
  _S::resizeEvent(event);
  resizeBoard();
}

QSize
ListScroll::sizeHint() const
{
  auto size = _board.sizeHint();
  auto border = frameWidth() << 1;
  size.setWidth(size.width() + border);
  size.setHeight(size.height() + border);
  return size;
}

}

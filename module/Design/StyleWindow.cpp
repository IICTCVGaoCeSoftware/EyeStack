#include "Eyestack/Design/StyleWindow.hpp"

namespace Eyestack::Design {

StyleWindow::StyleWindow(QWidget* parent)
  : _S(parent, Qt::FramelessWindowHint | Qt::CustomizeWindowHint)
  , _layout(this)
{
  // 使得即使作为底层窗口时背景也能完全透明
  setAttribute(Qt::WA_TranslucentBackground);

  _layout.setContentsMargins(0, 0, 0, 0);
  _layout.setSpacing(0);

  _layout.addWidget(&_titleBar, 0, Qt::AlignTop);
  _titleBar.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  _titleBar.setMinimumHeight(style()->pixelMetric(QStyle::PM_TitleBarHeight));
}

void
StyleWindow::paintEvent(QPaintEvent* event)
{
  _titleBar.text = windowTitle();
  _titleBar.icon = windowIcon();
  _titleBar.titleBarFlags = windowFlags();
  _titleBar.titleBarState = windowState();

  _S::paintEvent(event);
}

void
StyleWindow::TitleBar::paintEvent(QPaintEvent* event)
{
  initFrom(this);
  _S1::palette.setColor(QPalette::Window,
                        _S1::palette.color(QPalette::Highlight));

  QStylePainter sp(this);
  sp.drawComplexControl(QStyle::CC_TitleBar, *this);
  event->accept();
}

}

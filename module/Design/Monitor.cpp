#include "Eyestack/Design/Monitor.hpp"

namespace Eyestack::Design {

Monitor::Monitor(QWidget* parent)
  : _S(parent)
{
  resize(400, 300);

  // 设置屏幕
  _screenLabel.setFrameStyle(QFrame::Panel | QFrame::Sunken);

  // 设置说明文字，使用禁用来实现灰化效果
  _captionLabel.setEnabled(false);

  // 设置电源按钮
  _powerButton.setCheckable(true);

  QIcon icon;
  icon.addFile(
    ":/icon/power-off-solid green.svg", QSize(), QIcon::Normal, QIcon::On);
  icon.addFile(
    ":/icon/power-off-solid red.svg", QSize(), QIcon::Normal, QIcon::Off);
  _powerButton.setIcon(icon);

  // 布局界面
  auto hLayout = new QHBoxLayout();
  hLayout->addWidget(&_captionLabel);
  hLayout->addWidget(&_powerButton);

  auto vLayout = new QVBoxLayout();
  vLayout->addWidget(&_screenLabel);
  vLayout->addLayout(hLayout);

  setLayout(vLayout);

  // 连接信号
  connect(&_powerButton, &QToolButton::clicked, this, &_T::s_powerClicked);
  connect(
    &_powerButton, &QToolButton::clicked, this, &_T::when_powerButton_clicked);

  // 同步状态
  set_power(false);
}

}

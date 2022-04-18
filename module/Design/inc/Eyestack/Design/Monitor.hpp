#pragma once

#include "Eyestack/Design/ImageLabel.hpp"

namespace Eyestack::Design {

/**
 * @brief 一个简单的仿监视器组件，具有开关状态，可以设置说明文字，提供了一个
 * refresh 槽函数用于画面的刷新。
 */
class EYESTACK_DESIGN_EXPORT Monitor : public QWidget
{
  Q_OBJECT

  using _T = Monitor;
  using _S = QWidget;

public:
  Monitor(QWidget* parent = nullptr);

public:
  /**
   * @brief 获取组件的打开/关闭状态
   * @return 打开返回 true，关闭返回 false
   */
  bool get_power() { return _powerButton.isChecked(); }

  /**
   * @brief 获取组件左下角的说明文字
   * @return QString 对象
   */
  QString get_caption() { return _captionLabel.text(); }

  /**
   * @brief 获取当前监视器的画面
   * @return QPixmap 对象
   */
  const QPixmap& get_screen() { return _screenLabel.current(); }

public:
  /**
   * @brief 泛型的刷新监视器画面函数
   * @param image 要显示的图像
   */
  template<typename T>
  void display(T&& image)
  {
    if (get_power())
      _screenLabel.display(image);
  }

signals:
  /**
   * @brief 当用户点击电源按钮后会发射此信号
   * @param on 点击后，如果组件是打开的，为 true，否则为 false
   */
  void s_powerClicked(bool on);

public slots:
  /**
   * @brief 设置组件的打开/关闭状态
   * @param on 打开设为 true，关闭设为 false
   */
  void set_power(bool on)
  {
    _powerButton.setChecked(on);
    when_powerButton_clicked(on);
  }

  /**
   * @brief 设置组件左下角的说明文字
   */
  void set_caption(QString text) { _captionLabel.setText(text); }

  /**
   * @brief 设置监视器画面
   * @param image QImage 类型，而不是 QPixmap，因为后者的值传递开销太大
   */
  void set_screen(QImage image) { display(std::move(image)); }

  /**
   * @brief 显示 QString 文本，这个函数主要用作槽来连接使用
   */
  void display_qstring(QString text) { display(text); }

  /**
   * @brief 显示 QImage 图像，这个函数主要用作槽来连接使用
   */
  void display_qimage(QImage image) { display(image); }

  /**
   * @brief 显示 OpenCV Mat 图像，这个函数主要用作槽来连接使用
   */
  void display_cvmat(cv::Mat mat) { display(mat); }

private:
  ImageLabel _screenLabel;
  QToolButton _powerButton;
  QLabel _captionLabel;

private slots:
  void when_powerButton_clicked(bool checked)
  {
    if (!checked)
      _screenLabel.display(tr("监视器已关闭"));
    else
      _screenLabel.display(tr("无画面"));
  }
};

}

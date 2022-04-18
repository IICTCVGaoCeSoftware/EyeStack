#pragma once

#include "Eyestack/Base.hpp"

namespace Eyestack::Design {

/**
 * @brief 专用于图像显示的标签组件，也可显示说明文字，无论显示图片还是文字，
 * 都能自适应组件的实际大小，按比例缩放图像。提供了统一的 display 重载
 * 方法显示各种数据。
 */
class EYESTACK_DESIGN_EXPORT ImageLabel : public QFrame
{
  Q_OBJECT

  using _T = ImageLabel;
  using _S = QFrame;

public:
  using Color = QImage::Format;

public:
  ImageLabel(QWidget* parent = nullptr)
    : _S(parent)
    , _flag(0)
  {}

public:
  /**
   * @brief 获取当前显示图像
   * @return QPixmap 对象，如果当前显示的是文本，则返回空对象。
   */
  const QPixmap& current() { return _pixmap; }

  // Qt 图像类型支持
public:
  /**
   * @brief 显示 Qt Pixmap
   * @param image 图像
   */
  void display(const QPixmap& pixmap)
  {
    _pixmap = pixmap;
    update();
  }

  /**
   * @brief 显示 Qt 图像，该函数会对图像进行深拷贝
   * @param image 图像
   */
  void display(const QImage& image)
  {
    _pixmap = QPixmap::fromImage(image);
    update();
  }

  // cv::Mat 类型支持
public:
  /**
   * @brief 显示 OpenCV 图像，该函数会对图像进行深拷贝
   * @param mat 图像
   */
  void display(const cv::Mat& mat)
  {
    display(Eyestack::Base::util::cvmat_as_qimage(mat));
  }

  // 字符串类型支持
public:
  /**
   * @brief 显示本地编码的 C 字符串
   * @param text 以 '\0' 结尾的字符串指针。
   */
  void display(const char* text)
  {
    _text = text;
    _pixmap = QPixmap();
    update();
  }

  /**
   * @brief 显示 Qt 字符串
   * @param text 字符串对象，Qt 的字符串采用隐式共享，所以该方法无字
   * 符串拷贝操作。
   */
  void display(const QString& text)
  {
    _text = text;
    _pixmap = QPixmap();
    update();
  }

  /**
   * @brief 显示 C++ 标准库字符串
   * @param text 字符串对象。
   */
  void display(const std::string& text)
  {
    _text = QString::fromStdString(text);
    _pixmap = QPixmap();
    update();
  }

  /**
   * @brief 设置显示文本时的字体，等同与调用 setFont。
   * @param font 字体
   */
  void set_text_font(const QFont& font) { setFont(font); }

  /**
   * @brief 设置显示文本时的颜色，等同于调用 palette()->setColor。
   * @param color 颜色
   */
  void set_text_color(const QColor& color)
  {
    auto p = palette();
    p.setColor(QPalette::WindowText, color);
    setPalette(p);
  }

  /**
   * @brief 获取文本绘制标志位
   * @param flag 标志位，Qt::TextFlag 或 0
   */
  int get_text_flag() { return _flag; }

  /**
   * @brief 设置文本绘制标志位
   * @param flag 标志位
   */
  void set_text_flag(Qt::TextFlag flag)
  {
    _flag = flag;
    if (_pixmap.isNull())
      update();
  }

public slots:
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
  QPixmap _pixmap; // QPixmap 的绘制效率比 QImage 高
  QString _text;   // 只有当 _pixmap 不为空时才显示文本
  int _flag;

  // QWidget interface
protected:
  virtual void paintEvent(QPaintEvent* event) override;
  virtual QSize sizeHint() const override;
};

}

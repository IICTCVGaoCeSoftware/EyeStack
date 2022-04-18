#include "Eyestack/Design/ImageBench.hpp"
#include "Eyestack/Asset.hpp"

namespace Eyestack::Design {

static inline QString
n2s(int n)
{
  return QString::number(n);
}

// ==========================================================================
// ImageBenchBase
// ==========================================================================

void
ImageBenchBase::setup_ui()
{
  auto vlayout = new QVBoxLayout();
  setLayout(vlayout);

  vlayout->addWidget(&_view);

  auto hlayout = new QHBoxLayout();
  vlayout->addLayout(hlayout);

  hlayout->addWidget(&_scaleLabel);

  auto vline = new QFrame();
  vline->setFrameShape(QFrame::VLine);
  vline->setFrameShadow(QFrame::Sunken);
  hlayout->addWidget(vline);

  hlayout->addWidget(&_mouseLabel);
  _mouseLabel.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  vline = new QFrame();
  vline->setFrameShape(QFrame::VLine);
  vline->setFrameShadow(QFrame::Sunken);
  hlayout->addWidget(vline);

  hlayout->addWidget(&_infoLabel);
}

void
ImageBenchBase::update_info_from_qpixmap(const QPixmap& img)
{
  // 格式示例："1920x768 D1AB"
  _infoLabel.setText(n2s(img.width()) + "x" + n2s(img.height()) + " D" +
                     n2s(img.depth()) + (img.hasAlpha() ? "A" : "") +
                     (img.isQBitmap() ? "B" : ""));

  // 调整视野到图片
  _view.fitInView(img.rect(), Qt::KeepAspectRatio);
}

void
ImageBenchBase::update_info_from_qimage(const QImage& img)
{
  static const char* kFormatTranslations[] = { " Invalid",
                                               " Mono",
                                               " MonoLSB",
                                               " Indexed8",
                                               " RGB32",
                                               " ARGB32",
                                               " ARGB32_Premultiplied",
                                               " RGB16",
                                               " ARGB8565_Premultiplied",
                                               " RGB666",
                                               " ARGB6666_Premultiplied",
                                               " RGB555",
                                               " ARGB8555_Premultiplied",
                                               " RGB888",
                                               " RGB444",
                                               " ARGB4444_Premultiplied",
                                               " RGBX8888",
                                               " RGBA8888",
                                               " RGBA8888_Premultiplied",
                                               " BGR30",
                                               " A2BGR30_Premultiplied",
                                               " RGB30",
                                               " A2RGB30_Premultiplied",
                                               " Alpha8",
                                               " Grayscale8",
                                               " RGBX64",
                                               " RGBA64",
                                               " RGBA64_Premultiplied",
                                               " Grayscale16",
                                               " BGR888" };
  QString format;
  if (int(img.format()) < sizeof(kFormatTranslations) / sizeof(const char*))
    format = kFormatTranslations[img.format()];
  else
    format = " ERROR";
  _infoLabel.setText(n2s(img.width()) + "x" + n2s(img.height()) + format);

  // 调整视野到图片
  _view.fitInView(img.rect(), Qt::KeepAspectRatio);
}

void
ImageBenchBase::update_info_from_cvmat(const cv::Mat& mat)
{
  // 格式示例："1920x768 8UC1"
  _infoLabel.setText(n2s(mat.cols) + "x" + n2s(mat.rows) + " " +
                     QString::fromStdString(cv::typeToString(mat.type())));

  // 调整视野到图片
  _view.fitInView(0, 0, mat.cols, mat.rows, Qt::KeepAspectRatio);
}

// ==========================================================================
// ImageBenchBase::View
// ==========================================================================

ImageBenchBase::View::View(ImageBenchBase& self)
  : _S()
  , _self(self)
  , _scale(1)
  , _dragging(false)
{
  setMinimumSize(200, 200);
  setMouseTracking(true);
  setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

  _mapTool.setIcon(Asset::faicon("map-marked-alt-solid"));
  setCornerWidget(&_mapTool);
  connect(&_mapTool, &QToolButton::clicked, [this]() {
    fitInView(sceneRect(), Qt::KeepAspectRatio);
  });

  // 同步 ImageBenchBase 的状态
  scale(1);
}

void
ImageBenchBase::View::scale(qreal factor)
{
  _S::scale(factor, factor);
  _scale *= factor;
  _self._scaleLabel.setText(QString::number(_scale * 100, 'f', 0) + " %");
}

void
ImageBenchBase::View::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::MiddleButton) {
    _dragging = true;
    _mouseOrigin = mapToScene(event->pos());
  }

  _S::mousePressEvent(event);
}

void
ImageBenchBase::View::mouseReleaseEvent(QMouseEvent* event)
{
  if (event->button() == Qt::MiddleButton) {
    _dragging = false;
  }

  _S::mouseReleaseEvent(event);
}

void
ImageBenchBase::View::mouseMoveEvent(QMouseEvent* event)
{
  auto pos = event->pos();
  auto scenePos = mapToScene(pos);

  if (_dragging) {
    // 拖放画面
    auto centerPos = mapToScene(viewport()->rect().center());
    centerOn(centerPos - (scenePos - _mouseOrigin)); // 向鼠标的相反方向移动

  } else {
    // 刷新 mouseLabel，格式示例："(31,42) [120,23,54,32]"
    auto text = '(' + n2s(scenePos.x()) + ',' + n2s(scenePos.y()) + ')';

    QImage image(1, 1, QImage::Format_ARGB32);
    QPainter painter(&image);
    render(&painter, { 0, 0, 1, 1 }, { pos.x(), pos.y(), 1, 1 });
    auto color = image.pixelColor(0, 0);
    text += " [" + n2s(color.alpha()) + ',' + n2s(color.red()) + ',' +
            n2s(color.green()) + ',' + n2s(color.blue()) + ']';

    _self._mouseLabel.setText(text);
  }

  _S::mouseMoveEvent(event);
}

void
ImageBenchBase::View::wheelEvent(QWheelEvent* event)
{
  if (event->angleDelta().y() > 0)
    scale(1.1);
  else
    scale(0.9);
  event->accept();
}

// ==========================================================================
// ImageBenchBase::Scene
// ==========================================================================

void
ImageBenchBase::Scene::drawBackground(QPainter* painter, const QRectF& rect)
{
  _S::drawBackground(painter, rect);

  // QGraphicsView 会自动保存 Painter 的状态
  // painter->save();

  if (rect.top() <= 0 && rect.bottom() > 0) {
    painter->setPen(Qt::red);
    painter->drawLine(rect.left() - 1, 0, rect.right() + 1, 0);
  }

  if (rect.left() <= 0 && rect.right() > 0) {
    painter->setPen(Qt::green);
    painter->drawLine(0, rect.top() - 1, 0, rect.bottom() + 1);
  }
}

// ==========================================================================
// ImageBenchBase::ImageItem
// ==========================================================================

QRectF
ImageBenchBase::ImageItem::boundingRect() const
{
  return QRectF(-(_image.width() >> 1),
                -(_image.height() >> 1),
                _image.width() << 1,
                _image.height() << 1);
}

void
ImageBenchBase::ImageItem::paint(QPainter* painter,
                                 const QStyleOptionGraphicsItem* option,
                                 QWidget* widget)
{
  painter->drawImage(_image.rect(), _image, _image.rect());
}

QPainterPath
ImageBenchBase::ImageItem::shape() const
{
  QPainterPath path;
  path.addRect(_image.rect());
  return path;
}

// ==========================================================================
// ImageBench
// ==========================================================================

ImageBench::ImageBench(QWidget* parent)
  : _S(parent)
  , _scene()
  , _imageItem()
{
  _scene.addItem(&_imageItem);
  _view.setScene(&_scene);
}

ImageBench::~ImageBench() {}

const QImage&
ImageBench::get_image()
{
  return _imageItem.get_image();
}

void
ImageBench::set_image(const QImage& img)
{
  _imageItem.set_image(img);
}

void
ImageBench::set_image(QImage&& img)
{
  _imageItem.set_image(std::move(img));
}

}

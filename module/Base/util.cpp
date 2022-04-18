#include "Eyestack/Base.hpp"
#include "Eyestack/Base/util.hpp"

namespace Eyestack::Base::util {

namespace _private {

void
delete_UMatData(void* p)
{
  delete reinterpret_cast<cv::UMatData*>(p);
}

}

QImage::Format
cvmat_of_qimage_type(const cv::Mat& mat)
{
  switch (mat.type()) {
    case CV_8UC1:
      return QImage::Format_Grayscale8;

    case CV_8UC3:
      return QImage::Format_BGR888;
      break;

    case CV_8UC4:
      return QImage::Format_ARGB32;
      break;

    case CV_16UC1:
      return QImage::Format_Grayscale16;

    case CV_16UC4:
      return QImage::Format_RGBA64;

    default:
      return QImage::Format_Invalid;
  }
}

QImage
cvmat_to_qimage(const cv::Mat& mat)
{
  auto fmt = cvmat_of_qimage_type(mat);
  if (fmt != QImage::Format_Invalid)
    return cvmat_as_qimage(mat, fmt).copy();

  int dtype;

  switch (mat.channels()) {
    case 1:
      dtype = CV_8UC1;
      fmt = QImage::Format_Grayscale8;
      break;

    case 3:
      dtype = CV_8UC3;
      fmt = QImage::Format_BGR888;
      break;

    case 4:
      dtype = CV_8UC4;
      fmt = QImage::Format_ARGB32;
      break;

    default:
      return QImage();
  }

  cv::Mat dst;
  mat.convertTo(dst, dtype);

  QImage img(mat.data,
             mat.cols,
             mat.rows,
             mat.step,
             fmt,
             _private::delete_UMatData,
             mat.u); // Mat 的图像是存在 UMatData 里的，而不是 char[]

  dst.addref();
  dst.release();
  return img;
}

QImage
cvmat_to_qimage(cv::Mat&& mat)
{
  if (mat.u == nullptr)
    return QImage();

  if (mat.u->refcount != 1)
    return cvmat_to_qimage(mat);

  auto fmt = cvmat_of_qimage_type(mat);
  if (fmt == QImage::Format_Invalid)
    return cvmat_to_qimage(mat);

  QImage qimg(mat.data,
              mat.cols,
              mat.rows,
              mat.step,
              fmt,
              _private::delete_UMatData,
              mat.u); // Mat 的图像是存在 UMatData 里的，而不是 char[]

  mat.addref();
  mat.release();
  return qimg;
}

QImage
cvmat_to_qimage_norm(const cv::Mat& mat,
                     double alpha,
                     double beta,
                     int normType)
{
  int dtype;
  QImage::Format fmt;

  switch (mat.channels()) {
    case 1:
      dtype = CV_8UC1;
      fmt = QImage::Format_Grayscale8;
      break;

    case 3:
      dtype = CV_8UC3;
      fmt = QImage::Format_BGR888;
      break;

    case 4:
      dtype = CV_8UC4;
      fmt = QImage::Format_ARGB32;
      break;

    default:
      return QImage();
  }

  cv::Mat dst;
  cv::normalize(mat, dst, alpha, beta, normType, dtype);

  QImage img(mat.data,
             mat.cols,
             mat.rows,
             mat.step,
             fmt,
             _private::delete_UMatData,
             mat.u);

  dst.addref();
  dst.release();
  return img;
}

int
qimage_of_cvmat_type(const QImage& img)
{
  switch (img.format()) {
    case QImage::Format_Alpha8:
    case QImage::Format_Grayscale8:
      return CV_8UC1;

    case QImage::Format_RGB888:
    case QImage::Format_BGR888:
      return CV_8UC3;

    case QImage::Format_RGB32:
    case QImage::Format_ARGB32:
    case QImage::Format_RGBA8888:
    case QImage::Format_RGBX8888:
      return CV_8UC4;

    case QImage::Format_Grayscale16:
      return CV_16UC1;

    case QImage::Format_RGBA64:
    case QImage::Format_RGBX64:
      return CV_16UC4;

    default:
      return -1;
  }
}


cv::Mat
qimage_to_cvmat(const QImage& img)
{
  cv::Mat mat;

  switch (img.format()) {
      // 可以直接转换的类型
    case QImage::Format_Invalid:
      return cv::Mat();

    case QImage::Format_Grayscale8:
      mat = qimage_as_cvmat(img, CV_8UC1);
      break;

    case QImage::Format_BGR888:
      mat = qimage_as_cvmat(img, CV_8UC3);
      break;

    case QImage::Format_ARGB32:
      mat = qimage_as_cvmat(img, CV_8UC4);
      break;

      // 需要间接转换的类型
    case QImage::Format_Mono:
    case QImage::Format_MonoLSB:
    case QImage::Format_Grayscale16:
      mat = qimage_as_cvmat(img.convertToFormat(QImage::Format_Grayscale8),
                            CV_8UC1);
      break;

    case QImage::Format_Indexed8:
    case QImage::Format_RGB32:
    case QImage::Format_RGB16:
    case QImage::Format_RGB666:
    case QImage::Format_RGB555:
    case QImage::Format_RGB888:
    case QImage::Format_RGB444:
    case QImage::Format_RGBX8888:
    case QImage::Format_BGR30:
    case QImage::Format_RGB30:
    case QImage::Format_RGBX64:
      mat =
        qimage_as_cvmat(img.convertToFormat(QImage::Format_BGR888), CV_8UC3);
      break;

    default:
      mat =
        qimage_as_cvmat(img.convertToFormat(QImage::Format_ARGB32), CV_8UC4);
      break;
  }

  return mat.clone();
}

void
exec_widget(QWidget& widget)
{
  QDialog dlg;
  QHBoxLayout layout(&dlg);
  layout.addWidget(&widget);
  dlg.exec();
  widget.setParent(nullptr);
}

namespace serialize {

QDataStream&
operator<<(QDataStream& ds, QFile& file)
{
  if (file.isOpen())
    file.close();

  file.open(QIODevice::ReadOnly);

  ds << file.size();

  char* buf = new char[8192];
  qint64 size;
  while (0 != (size = file.read(buf, 8192)))
    ds.writeBytes(buf, size);
  delete[] buf;

  file.close();

  return ds;
}

QDataStream&
operator>>(QDataStream& ds, QFile& file)
{
  if (file.isOpen())
    file.close();

  file.open(QIODevice::WriteOnly);

  qint64 size;
  ds >> size;

  char* buf = new char[8192];
  while (size > 0) {
    auto read = ds.readRawData(buf, 8192);
    file.write(buf, read);
  }
  delete[] buf;

  file.close();

  return ds;
}

QDataStream&
operator<<(QDataStream& ds, cv::InputArray arr)
{
  cv::Mat mat = arr.getMat();

  ds << mat.type() << mat.rows << mat.cols;

  if (mat.isContinuous()) {
    ds.writeRawData(mat.ptr<char>(), (mat.dataend - mat.datastart));

  } else {
    int rowsz = CV_ELEM_SIZE(mat.type()) * mat.cols;
    for (int r = 0; r < mat.rows; ++r)
      ds.writeRawData(mat.ptr<char>(r), rowsz);
  }

  return ds;
}

QDataStream&
operator>>(QDataStream& ds, cv::OutputArray arr)
{
  int type, rows, cols;
  ds >> type >> rows >> cols;

  arr.create(rows, cols, type);
  cv::Mat mat = arr.getMat();

  int size = CV_ELEM_SIZE(type) * rows * cols;
  int realSize = ds.readRawData(mat.ptr<char>(), size);

  if (realSize != size)
    throw Base::exc::LoadingError(
      QObject::tr("cv::Mat 数据长度不符合预期： %1 < %2")
        .arg(realSize)
        .arg(size));

  return ds;
}

}
}

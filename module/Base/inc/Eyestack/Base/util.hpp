#pragma once

#include "exc.hpp"

/**
 * @brief 这个命名空间下放的都是独立、无副作用的纯函数
 */
namespace Eyestack::Base::util {

namespace _private {
void
delete_UMatData(void* p);
}

// ==========================================================================
// cv::Mat 与 QImage 格式转换
// ==========================================================================

/**
 * @brief 求出与 cv::Mat 图像数据格式兼容的 QImage 格式
 * @return 如果没有兼容格式，则返回 QImage::Format_Invalid
 */
QImage::Format
cvmat_of_qimage_type(const cv::Mat& mat);

/**
 * @brief 包装 cv::Mat 为 QImage，小心指针悬挂。
 * @return 该函数可以避免拷贝 cv::Mat 的图像数据，但是调用者需保证在使用返回
 * 值时 mat 不会被析构。
 */
inline QImage
cvmat_as_qimage(const cv::Mat& mat, QImage::Format fmt)
{
  return QImage(mat.data, mat.cols, mat.rows, mat.step, fmt);
}

/**
 * @brief 包装 cv::Mat 为 QImage，自动识别格式，当心指针悬挂。该函数可以避
 * 免拷贝 cv::Mat 的图像数据，但是调用者需保证在使用返回值时 mat 不会被析构。
 * @return 如果转换失败，返回空的 QImage 对象。
 */
inline QImage
cvmat_as_qimage(const cv::Mat& mat)
{
  auto fmt = cvmat_of_qimage_type(mat);
  if (fmt == QImage::Format_Invalid)
    return QImage();
  return cvmat_as_qimage(mat, fmt);
}

/**
 * @brief cv::Mat 转 QImage，深拷贝图像数据。如果 mat 的格式没有直
 * 接对应的 QImage 格式，该函数会自动尝试进行类型转换，将数据舍入到最近的对应
 * 格式后转换为 QImage。
 * @return 如果转换失败，返回空的 QImage 对象。
 */
QImage
cvmat_to_qimage(const cv::Mat& mat);

/**
 * @brief cv::Mat 转 QImage，尽量避免拷贝图像数据。如果 mat 的图像引用计数
 * 不为 1，出于内存安全考虑，则该函数必定进行深拷贝。如果 mat 没有直接对应的
 * QImage 格式，则也进行深拷贝。如果转换成功，mat 将被置空。
 * @return 如果转换失败，返回空的 QImage 对象。
 */
QImage
cvmat_to_qimage(cv::Mat&& mat);

/**
 * @brief 将 cv::Mat 归一化后转换为 QImage，常用于将浮点类型的 cv::Mat
 * 转换为 QImage，不过也适用于其它整数类型。
 * 参数 alpha、beta、dtype 的作用同 cv::normalize 函数。
 * @return 返回的图像数据必定是 mat 的深拷贝，转换失败返回空的 QImage 对象。
 */
QImage
cvmat_to_qimage_norm(const cv::Mat& mat,
                     double alpha = 0,
                     double beta = 255,
                     int normType = cv::NORM_MINMAX);

/**
 * @brief 求出与 QImage 兼容的 cv::Mat 类型，注意兼容仅指数据结构的兼容，其
 * 色彩模式的高级语义可能不相同。
 * @return 如果没有兼容格式，则返回 -1
 */
int
qimage_of_cvmat_type(const QImage& img);

/**
 * @brief 包装 QImage 为 cv::Mat，小心指针悬挂。
 * @return 这个函数可以避免拷贝 QImage 的图像，但是调用者需保证在使用返回值时
 * img 不会被析构，即返回值的生命期包含在 img 的生命期中。
 */
inline cv::Mat
qimage_as_cvmat(const QImage& img, int type)
{
  // 这种方式构造的 cv::Mat 在析构时不会释放图像数据
  return cv::Mat(img.height(),
                 img.width(),
                 type,
                 const_cast<uchar*>(img.bits()),
                 img.bytesPerLine());
}

/**
 * @brief 包装 QImage 为 cv::Mat，自动识别格式，当心指针悬挂。
 * @return 该函数可以避免拷贝 QImage 的图像数据，但是调用者需保证在使用返回
 * 值时 img 不会被析构。
 */
inline cv::Mat
qimage_as_cvmat(const QImage& img)
{
  int type = qimage_of_cvmat_type(img);
  if (type == -1)
    return cv::Mat();
  return qimage_as_cvmat(img, type);
}

/**
 * @brief QImage 转 cv::Mat，深拷贝图像数据。该函数会自动尝试进行类型转换，
 * 如果转换成功，所得的 cv::Mat 类型可能为 CV_8CU1（灰度）、CV_8CU3（BGR）、
 * CV_8CU4（ARGB）。
 * @return 如果转换失败，返回空的 cv::Mat 对象。
 */
cv::Mat
qimage_to_cvmat(const QImage& img); // 没有 QImage&& 的优化版本

// ==========================================================================
// QWidget 执行器
// ==========================================================================

/**
 * @brief 将 widget 显示为模态对话框，阻塞执行直到组件被关闭
 * @param widget 组件对象的引用，如果其 _parent 不为空，执行完毕将被置空
 */
void
exec_widget(QWidget& widget);

// ==========================================================================
// QDebug 流类型拓展
// ==========================================================================

/**
 * @brief 将捕获异常输出到 QDebug 上
 */
inline QDebug
operator<<(QDebug dbg, std::exception_ptr e)
{
  try {
    std::rethrow_exception(e);

  } catch (const Base::exc::Exception& e) {
    dbg << e.repr();

  } catch (const std::exception& e) {
    dbg << e.what();

  } catch (...) {
    dbg << "<unkown std::exception_ptr>";
  }
  return dbg;
}

// ==========================================================================
// 常用的 QMessageBox 弹出模板
// ==========================================================================

namespace popmsg {

/**
 * @brief 临时的错误提示
 * @param file 建议使用 __FILE__ 宏
 * @param line 建议使用 __LINE__ 宏
 */
inline void
something_wrong(const char* file, int line, QWidget* parent = nullptr)
{
  QMessageBox::critical(
    parent,
    QMessageBox::tr("错误"),
    QMessageBox::tr("文件：%1\n行：%2").arg(file).arg(line));
}

/**
 * @brief 功能未实现
 */
inline void
function_not_implemented(const char* func,
                         const char* file,
                         int line,
                         QWidget* parent = nullptr)
{
  QMessageBox::critical(parent,
                        QMessageBox::tr("错误：功能未实现"),
                        QMessageBox::tr("函数：%1\n文件：%2\n行：%3")
                          .arg(func)
                          .arg(file)
                          .arg(line));
}

/**
 * @brief 程序执行到了意料之外的分支
 */
inline void
unexpected_brach(const char* file, int line, QWidget* parent = nullptr)
{
  QMessageBox::critical(
    parent,
    QMessageBox::tr("错误：意料之外的程序分支"),
    QMessageBox::tr("文件：%1\n行：%2").arg(file).arg(line));
}

/**
 * @brief 打开文件失败
 */
inline void
open_file_failed(const QString& path, QWidget* parent = nullptr)
{
  QMessageBox::critical(parent,
                        QMessageBox::tr("错误：打开文件失败"),
                        QMessageBox::tr("无效的文件路径：\"%1\"").arg(path));
}

}

// ==========================================================================
// 序列化工具函数
// ==========================================================================

namespace serialize {

QDataStream&
operator<<(QDataStream& ds, QFile& file);

QDataStream&
operator>>(QDataStream& ds, QFile& file);

QDataStream&
operator<<(QDataStream& ds, const cv::Mat& mat);

QDataStream&
operator>>(QDataStream& ds, cv::Mat& mat);

QDataStream&
operator<<(QDataStream& ds, cv::InputArray arr);

QDataStream&
operator>>(QDataStream& ds, cv::OutputArray arr);

inline QDataStream&
operator<<(QDataStream& ds, const cv::Size& size)
{
  ds << size.width << size.height;
  return ds;
}

inline QDataStream&
operator>>(QDataStream& ds, cv::Size& size)
{
  ds >> size.width >> size.height;
  return ds;
}

}

}

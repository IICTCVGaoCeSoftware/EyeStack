#pragma once

#include "Eyestack/Design/Monitor.hpp"
#include "Eyestack/Framework.hpp"
#include "Eyestack/Gencom/base.hpp"

namespace Eyestack::Gencom {

class VideoCaptureUi;

// ==========================================================================
// VideoCapture
// ==========================================================================

/**
 * @brief cv::VideoCapture 的包装组件，提供了选择数据源的用户界面。
 *
 * 打开、关闭、读取图像等操作是线程安全的。
 */
class EYESTACK_GENCOM_EXPORT VideoCapture
  : public QObject
  , private GinitRequired
{
  Q_OBJECT

  using _T = VideoCapture;
  using _S = QObject;

public:
  class SubUi;

public:
  VideoCapture(Framework::Worker& worker)
    : _S(&worker)
  {}

public:
  /**
   * @brief 获取所处的工作对象
   * @threadsafe
   */
  Framework::Worker& worker()
  {
    return *dynamic_cast<Framework::Worker*>(_S::parent());
  }

  /**
   * @brief open 的字符串重载支持图片、视频、相机、网络视频流等多种数据源，详见
   * OpenCV 官方文档
   * @threadsafe
   */
  bool open(const char* path)
  {
    QMutexLocker locker(&_mutex);
    _cameraIndex = -1, _sourcePath = path;
    return open();
  }

  bool open(const cv::String& path)
  {
    QMutexLocker locker(&_mutex);
    _cameraIndex = -1, _sourcePath = QString::fromStdString(path);
    return open();
  }

  bool open(const QString& path)
  {
    QMutexLocker locker(&_mutex);
    _cameraIndex = -1, _sourcePath = path;
    return open();
  }

  /**
   * @brief 打开相机
   * @param index 从 0 开始的相机编号
   * @threadsafe
   */
  bool open(int index)
  {
    QMutexLocker locker(&_mutex);
    _cameraIndex = index;
    return open();
  }

  /**
   * @brief 重新打开之前使用的数据源
   * @threadsafe
   */
  bool open()
  {
    QMutexLocker locker(&_mutex);
    return update_source();
  }

  /**
   * @brief 关闭数据源，释放之前打开的资源
   * @threadsafe
   */
  void close()
  {
    QMutexLocker locker(&_mutex);
    _capture.release();
  }

  /**
   * @brief 数据源已打开返回 true，否则返回 false
   * @threadsafe
   */
  bool is_opened()
  {
    QMutexLocker locker(&_mutex);
    return _capture.isOpened();
  }

  /**
   * @brief 读取一帧图片，如果监视启用，则会发射 refresh 信号
   * @param image 输出图像指针
   * @return 成功返回 true，失败返回 false
   * @threadsafe
   */
  bool read(cv::Mat* image);

  /**
   * @brief 获取当前使用的相机编号，如果当前数据源不是相机，则返回 -1
   * @threadsafe
   */
  int camera()
  {
    QMutexLocker locker(&_mutex);
    return _cameraIndex;
  }

  /**
   * @brief 获取当前使用的路径字符串，如果当前使用数据源类型不符，返回空字符串
   * @threadsafe
   */
  QString path()
  {
    QMutexLocker locker(&_mutex);
    return _cameraIndex == -1 ? QString() : _sourcePath;
  }

  /**
   * @brief 获取监视启用状态
   * @return 若启用则返回 true，否则返回 false。
   * @threadsafe
   */
  bool get_monitored() { return _monitored.load(); }

  /**
   * @brief 不发射更新信号，直接设置组件的监视启用状态
   * @param flag 为 true 启用，为 false 禁用。
   * @threadsafe
   */
  void set_monitored(bool flag) { _monitored.store(flag); }

signals:
  /**
   * @brief 组件的监视状态被更新
   *
   * 注意，“更新”仅表示状态被重写，更新前后的值可能实际上是相等的。
   */
  void s_monitoredUpdated(bool flag, QPrivateSignal = QPrivateSignal());

  /**
   * @brief 当监视启用时，每次组件从数据源读取一帧图像都会发送此信号
   */
  void s_refresh(::cv::Mat frame, QPrivateSignal = QPrivateSignal());

public slots:
  /**
   * @brief 设置组件监视状态，并发射更新信号
   * @param flag 为 true 启用，为 false 禁用。
   */
  void setMonitored(bool flag)
  {
    set_monitored(flag);
    emit s_monitoredUpdated(flag);
  }

private:
  QMutex _mutex{ QMutex::Recursive };
  cv::VideoCapture _capture;
  int _cameraIndex{ -1 };
  QString _sourcePath;
  std::atomic_bool _monitored{ false };

private:
  // 刷新数据源，返回打开成功与否
  virtual bool update_source();
};

// ==========================================================================
// VideoCapture::SubUi
// ==========================================================================

/**
 * @brief VideoCapture 内置的界面类
 */
class EYESTACK_GENCOM_EXPORT VideoCapture::SubUi
  : public Framework::MainWindow::SubUi::MdiAction
{
  Q_OBJECT

  using _T = SubUi;
  using _S = Framework::MainWindow::SubUi::MdiAction;

public:
  SubUi(VideoCapture& com,
        const QString& name,
        const QIcon& icon = Asset::faicon("camera-solid"));

private:
  VideoCapture& _com;

  QWidget _centralWidget;
  QVBoxLayout _mainLayout;
  QHBoxLayout _buttonLayout;
  QPushButton _fileButton, _cameraButton, _customButton, _refreshButton;
  Design::ImageLabel _imageLabel;
  QCheckBox _monitoredCheck;

private slots:
  //  void disable_buttons();
  //  void enable_buttons();

  void when_fileButton_clicked();
  void when_cameraButton_clicked();
  void when_customButton_clicked();
  void when_refreshButton_clicked();
};

}

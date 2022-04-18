// 对应 MVS SDK 版本 3.4.2

#pragma once

#include "CameraParams.h"
#include "Eyestack/Framework.hpp"

//#define EYESTACK_COM_MVS_TEST

namespace Eyestack::Com_MVS {

/**
 * @threadsafe
 * @brief MVS 相机包装类，采用隐式共享模式。
 */
class EYESTACK_COM_MVS_EXPORT MvsCameraWrapper
{
  using _T = MvsCameraWrapper;

public:
  /**
   * @threadsafe
   * @brief 调用 MVS SDK，列出所有相机
   * @return 基于共享指针模式创建的 MvsCameraWrapper 对象们
   */
  static QVector<MvsCameraWrapper> list_all();

public:
  MvsCameraWrapper()
    : _data(new Data())
  {}

  MvsCameraWrapper(const MvsCameraWrapper& other)
    : _data(other._data)
  {}

public:
  /**
   * @brief 判断包装器共享指针是否有效
   */
  operator bool() { return _data->_handle; }

  /**
   * @brief 判断两个对象管理的是不是同一个相机控制块。
   */
  bool operator==(const MvsCameraWrapper& other)
  {
    return _data == other._data;
  }

public:
  /**
   * @threadsafe
   * @brief 获取相机的 MAC 地址
   */
  uint64_t mac_addr() const;

  /**
   * @threadsafe
   * @brief 获取相机名
   */
  QString name() const;

  /**
   * @threadsafe
   * @brief 获取一个比较详细的描述相机信息的字符串
   */
  QString info() const;

  /**
   * @threadsafe
   * @brief 打开相机
   */
  void open() noexcept(false);

  /**
   * @threadsafe
   * @brief 开始取流
   *
   * @param nodeNum 缓冲节点数量
   */
  void start_grabbing(unsigned int nodeNum = 3) noexcept(false);

  /**
   * @threadsafe
   * @brief 停止取流
   */
  void stop_grabbing() noexcept(false);

  /**
   * @threadsafe
   * @brief 关闭相机
   */
  void close() noexcept(false);

  /**
   * @threadsafe
   * @brief 检查相机是否是开启的
   */
  bool is_connected() noexcept(false);

  /**
   * @threadsafe
   * @brief 阻塞式拍摄相机画面（cv::Mat 版本）
   *
   * @param timeout 超时时间，单位毫秒，默认为 100。
   * @param colored 是否转彩色格式。
   * @return 返回一个新的 cv::Mat，它独占指向的图像数据（引用计数为 1），
   * 如果是彩色图像，矩阵的类型为 CV_8UC3（BGR 颜色模式），否则为灰度图，
   * 类型为 CV_8UC1。
   */
  cv::Mat snap_cvmat(uint timeout = 100, bool colored = false) noexcept(false);

  /**
   * @threadsafe
   * @brief 阻塞式拍摄相机画面（QImage 版本）
   *
   * @param timeout 超时时间，单位毫秒，默认为 100。
   * @param colored 是否转彩色格式。
   * @return 返回一个新的 QImage，它独占指向的图像数据（引用计数为 1）。
   */
  QImage snap_qimage(uint timeout = 100, bool colored = false) noexcept(false);

  /**
   * @threadsafe
   * @brief 保存相机属性到文件，相机必须是打开的。
   * @param path 文件路径
   */
  void save_feature(const QString& path) noexcept(false);

  /**
   * @threadsafe
   * @brief 从文件加载相机属性，相机必须是打开的。
   * @param path 文件路径
   */
  void load_feature(const QString& path) noexcept(false);

private:
  struct Data
  {
    char* _handle{ nullptr };
    MV_CC_DEVICE_INFO _info;
    QVector<unsigned char> _payload;
    QMutex _mutex;

#ifdef EYESTACK_COM_MVS_TEST
    int _frameCounter{ 0 };
#endif

    Data() { memset(&_info, 0, sizeof(_info)); }
    ~Data();
  };

private:
  QSharedPointer<Data> _data;
};

/**
 * @brief MVS 错误代码的异常包装类，提供了代码翻译功能
 */
class EYESTACK_COM_MVS_EXPORT MvsError : public Base::exc::Exception
{
public:
  static const QHash<int, const char*> kCodeTable;

public:
  MvsError()
    : _code(0)
  {}

  MvsError(int code)
    : _code(code)
  {}

  MvsError(const MvsError& other)
    : _code(other._code)
  {}

public:
  MvsError& operator=(const MvsError& other)
  {
    _code = other._code;
    return *this;
  }

public:
  int code() { return _code; }
  QString translate() const;

private:
  int _code;

  // Exception interface
public:
  virtual QString repr() const noexcept override;
};

}

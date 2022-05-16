// 对应 MVS SDK 版本 3.4.2

#pragma once

#include "CameraParams.h"
#include "Eyestack/Framework.hpp"

//#define EYESTACK_COM_MVS_TEST

namespace Eyestack::Com_MVS {

/**
 * @threadsafe
 * @brief MVS 相机包装类，不可移动，不可复制。
 */
class EYESTACK_COM_MVS_EXPORT MvsCameraWrapper final
{
  using _T = MvsCameraWrapper;

  MvsCameraWrapper(const MvsCameraWrapper& other) = delete;
  MvsCameraWrapper(MvsCameraWrapper&& other) = delete;

public:
  /**
   * @brief 共享指针类别名
   */
  using Shared = QSharedPointer<MvsCameraWrapper>;

public:
  /**
   * @threadsafe
   * @brief 调用 MVS SDK，列出所有相机
   * @return 基于共享指针模式创建的 MvsCameraWrapper 对象们
   */
  static QVector<Shared> list_all() noexcept(false);

public:
  MvsCameraWrapper() = default;
  ~MvsCameraWrapper() noexcept;

public:
  /**
   * @threadsafe
   * @brief 获取相机的 MAC 地址
   */
  uint64_t mac_addr() const noexcept;

  /**
   * @threadsafe
   * @brief 获取相机名
   */
  QString name() const noexcept;

  /**
   * @threadsafe
   * @brief 获取一个比较详细的描述相机信息的字符串
   */
  QString info() const noexcept;

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

  // ch:获取和设置Enum型参数
  void GetEnumValue(const char* strKey, MVCC_ENUMVALUE* pEnumValue);
  void SetEnumValue(const char* strKey, unsigned int nValue);
  // ch:获取和设置Float型参数
  void GetFloatValue(const char* strKey, MVCC_FLOATVALUE* pFloatValue);
  void SetFloatValue(const char* strKey, float fValue);
  // ch:执行一次Command型命令
  void CommandExecute(const char* strKey);
  // ch:保存图片 | en:save image
  void SaveImage(MV_SAVE_IMAGE_PARAM_EX* pstParam);
  // ch:保存图片为文件 | en:Save the image as a file
  void SaveImageToFile(MV_SAVE_IMG_TO_FILE_PARAM* pstSaveFileParam);

  //  /**
  //   * @threadsafe
  //   * @brief 检查相机是否已开始取流，如果相机没有打开，返回 false
  //   */
  //  bool is_grabbing() noexcept(false);

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
  void* _handle{ nullptr };
  QMutex _mutex;
  MV_CC_DEVICE_INFO _info{ 0 };
  QVector<unsigned char> _payload;
  //  bool _grabbing{ false };

#ifdef EYESTACK_COM_MVS_TEST
  bool _grabbing{ false };
  int _frameCounter{ 0 };
#endif
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

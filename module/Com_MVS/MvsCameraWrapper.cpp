#include "Eyestack/Com_MVS/MvsCameraWrapper.hpp"
#include "MvCameraControl.h"

#ifdef EYESTACK_COM_MVS_TEST
#include <QRandomGenerator>
#endif

namespace Eyestack::Com_MVS {

#ifndef EYESTACK_COM_MVS_TEST

/**
 * @brief 将返回的错误状态码转换为异常。
 */
static inline void
ensure(int code)
{
  if (MV_OK != code)
    throw MvsError(code);
}

QVector<MvsCameraWrapper>
MvsCameraWrapper::list_all()
{
  static QMutex mutex;
  QMutexLocker locker(&mutex);

  MV_CC_DEVICE_INFO_LIST deviceList;
  ensure(MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_1394_DEVICE | MV_USB_DEVICE |
                             MV_CAMERALINK_DEVICE,
                           &deviceList));

  QVector<MvsCameraWrapper> ret(deviceList.nDeviceNum);
  for (unsigned int i = 0; i < deviceList.nDeviceNum; ++i) {
    void* handle;
    MV_CC_CreateHandle(&handle, deviceList.pDeviceInfo[i]);

    ret[i]._data->_handle = reinterpret_cast<char*>(handle);
    MV_CC_GetDeviceInfo(handle, &ret[i]._data->_info);
  }

  return ret;
}

#else

static QRandomGenerator sRg;

QVector<MvsCameraWrapper>
MvsCameraWrapper::list_all()
{
  QVector<MvsCameraWrapper> _wrappers;

  auto len = sRg.bounded(0, 15);
  _wrappers.resize(len);

  for (int i = 0; i < len; ++i) {
    auto& d = _wrappers[i]._data;
    d->_info.nMacAddrLow = i;
    switch (sRg.bounded(0, 3)) {
      case 0:
        d->_info.nTLayerType = MV_USB_DEVICE;
        break;

      case 1:
        d->_info.nTLayerType = MV_GIGE_DEVICE;
        break;

      case 2:
        d->_info.nTLayerType = MV_1394_DEVICE;
        break;
    }
  }

  return _wrappers;
}

#endif

uint64_t
MvsCameraWrapper::mac_addr() const
{
  auto& deviceInfo = _data->_info;
  uint64_t macAddr = deviceInfo.nMacAddrHigh;
  macAddr = macAddr << 32 | deviceInfo.nMacAddrLow;
  return macAddr;
}

static inline QTextStream&
operator<<(QTextStream& out, unsigned char* p)
{
  return out << reinterpret_cast<char*>(p);
}

QString
MvsCameraWrapper::name() const
{
  QString ret;
  QTextStream sout(&ret);

  auto info = _data->_info;

  switch (info.nTLayerType) {
    case MV_GIGE_DEVICE: {
      sout << "[GIGE] ";
    } break;

    case MV_1394_DEVICE: {
      sout << "[1394] ";
    } break;

    case MV_USB_DEVICE: {
      sout << "[USB] ";
    } break;

    case MV_CAMERALINK_DEVICE: {
      sout << "[CAML] ";
    } break;

    default: {
      sout << "[UNKNOWN] ";
    } break;
  }

  sout << Qt::hex;
  sout.setPadChar('0');
  sout.setFieldWidth(8);
  sout << info.nMacAddrHigh;
  sout.setFieldWidth(0);
  sout << '-';
  sout.setFieldWidth(8);
  sout << info.nMacAddrLow;
  return ret;
}

QString
MvsCameraWrapper::info() const
{
  if (!_data)
    throw MvsError(MV_E_HANDLE);

  auto info = _data->_info;

  QString ret;
  QTextStream sout(&ret);

  switch (info.nTLayerType) {
    case MV_GIGE_DEVICE: {
      auto& gige = info.SpecialInfo.stGigEInfo;

      sout << "<MV_GIGE_DEVICE>";
      sout << "\nchDeviceVersion: " << gige.chDeviceVersion;
      sout << "\nchManufacturerName: " << gige.chManufacturerName;
      sout << "\nchManufacturerSpecificInfo: "
           << gige.chManufacturerSpecificInfo;
      sout << "\nchModelName: " << gige.chModelName;
      sout << "\nchSerialNumber: " << gige.chSerialNumber;
      sout << "\nchUserDefinedName: " << gige.chUserDefinedName;

      auto& ip = gige.nCurrentIp;
      sout << "\nnCurrentIp: " << (0xff & (ip >> 24)) << '.'
           << (0xff & (ip >> 16)) << '.' << (0xff & (ip >> 8)) << '.'
           << (0xff & ip);
    } break;

    case MV_1394_DEVICE: {
      sout << "<MV_1394_DEVICE>";
      sout << "nMajorVer: " << info.nMajorVer;
      sout << "nMinorVer: " << info.nMinorVer;
    } break;

    case MV_USB_DEVICE: {
      auto& usb3 = info.SpecialInfo.stUsb3VInfo;

      sout << "<MV_USB_DEVICE>";
      sout << "\nchDeviceGUID: " << usb3.chDeviceGUID;
      sout << "\nchDeviceVersion: " << usb3.chDeviceVersion;
      sout << "\nchFamilyName: " << usb3.chFamilyName;
      sout << "\nchManufacturerName: " << usb3.chManufacturerName;
      sout << "\nchModelName: " << usb3.chModelName;
      sout << "\nchSerialNumber: " << usb3.chSerialNumber;
      sout << "\nchUserDefinedName: " << usb3.chUserDefinedName;
      sout << "\nchVendorName: " << usb3.chVendorName;
    } break;

    case MV_CAMERALINK_DEVICE: {
      auto& caml = info.SpecialInfo.stCamLInfo;

      sout << "<MV_CAMERALINK_DEVICE>";
      sout << "\nchDeviceVersion: " << caml.chDeviceVersion;
      sout << "\nchFamilyName: " << caml.chFamilyName;
      sout << "\nchManufacturerName: " << caml.chManufacturerName;
      sout << "\nchModelName: " << caml.chModelName;
      sout << "\nchPortID: " << caml.chPortID;
      sout << "\nchSerialNumber: " << caml.chSerialNumber;
    } break;

    default: {
      sout << "<Unkown nTLayerType>";
    } break;
  }

  return ret;
}

#ifndef EYESTACK_COM_MVS_TEST

void
MvsCameraWrapper::open() noexcept(false)
{
  QMutexLocker locker(&_data->_mutex);

  auto handle = _data->_handle;
  ensure(MV_CC_OpenDevice(handle));

  if (_data->_info.nTLayerType == MV_GIGE_DEVICE) {
    int nPacketSize = MV_CC_GetOptimalPacketSize(handle);
    ensure(MV_CC_SetIntValue(handle, "GevSCPSPacketSize", nPacketSize));
  }

  // 获取数据包大小
  MVCC_INTVALUE param;
  ensure(MV_CC_GetIntValue(_data->_handle, "PayloadSize", &param));
  _data->_payload.resize(param.nCurValue);
}

void
MvsCameraWrapper::start_grabbing(unsigned int nodeNum) noexcept(false)
{
  QMutexLocker locker(&_data->_mutex);

  // 设置触发模式为off
  //  ensure(MV_CC_SetEnumValue(_handle, "TriggerMode", 0));

  // 在此处启动取流，避免每次snap时的开销
  ensure(MV_CC_SetImageNodeNum(_data->_handle, nodeNum));
  ensure(MV_CC_StartGrabbing(_data->_handle));
}

void
MvsCameraWrapper::stop_grabbing() noexcept(false)
{
  QMutexLocker locker(&_data->_mutex);

  ensure(MV_CC_StopGrabbing(_data->_handle));
}

void
MvsCameraWrapper::close() noexcept(false)
{
  QMutexLocker locker(&_data->_mutex);

  ensure(MV_CC_CloseDevice(_data->_handle));
}

bool
MvsCameraWrapper::is_connected() noexcept(false)
{
  QMutexLocker locker(&_data->_mutex);

  return MV_CC_IsDeviceConnected(_data->_handle);
}

cv::Mat
MvsCameraWrapper::snap_cvmat(uint timeout, bool colored) noexcept(false)
{
  QMutexLocker locker(&_data->_mutex);

  MV_FRAME_OUT_INFO_EX frameInfo;
  ensure(MV_CC_GetOneFrameTimeout(_data->_handle,
                                  _data->_payload.data(),
                                  unsigned(_data->_payload.size()),
                                  &frameInfo,
                                  timeout));
  cv::Mat img(frameInfo.nHeight, frameInfo.nWidth, colored ? CV_8UC3 : CV_8UC1);

  MV_CC_PIXEL_CONVERT_PARAM param;
  param.nWidth = frameInfo.nWidth;
  param.nHeight = frameInfo.nHeight;
  param.pSrcData = _data->_payload.data();
  param.nSrcDataLen = frameInfo.nFrameLen;
  param.enSrcPixelType = frameInfo.enPixelType;
  param.pDstBuffer = img.data;
  param.nDstBufferSize = (unsigned int)(img.total() * img.elemSize());
  param.enDstPixelType =
    colored ? PixelType_Gvsp_BGR8_Packed : PixelType_Gvsp_Mono8;
  ensure(MV_CC_ConvertPixelType(_data->_handle, &param));

  return img;
}

QImage
MvsCameraWrapper::snap_qimage(uint timeout, bool colored) noexcept(false)
{
  QMutexLocker locker(&_data->_mutex);

  MV_FRAME_OUT_INFO_EX frameInfo;
  ensure(MV_CC_GetOneFrameTimeout(_data->_handle,
                                  _data->_payload.data(),
                                  unsigned(_data->_payload.size()),
                                  &frameInfo,
                                  timeout));
  QImage img(frameInfo.nWidth,
             frameInfo.nHeight,
             colored ? QImage::Format_RGB888 : QImage::Format_Grayscale8);

  MV_CC_PIXEL_CONVERT_PARAM param;
  param.nWidth = frameInfo.nWidth;
  param.nHeight = frameInfo.nHeight;
  param.pSrcData = _data->_payload.data();
  param.nSrcDataLen = frameInfo.nFrameLen;
  param.enSrcPixelType = frameInfo.enPixelType;
  param.pDstBuffer = img.bits();
  param.nDstBufferSize = img.sizeInBytes();
  param.enDstPixelType =
    colored ? PixelType_Gvsp_RGB8_Packed : PixelType_Gvsp_Mono8;
  ensure(MV_CC_ConvertPixelType(_data->_handle, &param));

  return img;
}

void
MvsCameraWrapper::save_feature(const QString& path) noexcept(false)
{
  QMutexLocker locker(&_data->_mutex);

  auto tmp = path.toStdString();
  ensure(MV_CC_FeatureSave(_data->_handle, tmp.c_str()));
}

void
MvsCameraWrapper::load_feature(const QString& path) noexcept(false)
{
  QMutexLocker locker(&_data->_mutex);

  auto tmp = path.toStdString();
  ensure(MV_CC_FeatureLoad(_data->_handle, tmp.c_str()));
}

MvsCameraWrapper::Data::~Data()
{
  if (_handle)
    Q_ASSERT(MV_CC_DestroyHandle(_handle) == MV_OK);
}

#else

void
MvsCameraWrapper::open() noexcept(false)
{
  QMutexLocker locker(&_data->_mutex);

  if (sRg.generateDouble() < 0.01) {
    throw MvsError(MV_E_HANDLE);
  }
  _data->_handle = reinterpret_cast<char*>(1);
}

void
MvsCameraWrapper::start_grabbing(unsigned int nodeNum) noexcept(false)
{
  if (sRg.generateDouble() < 0.01) {
    throw MvsError(MV_E_HANDLE);
  }
}

void
MvsCameraWrapper::stop_grabbing() noexcept(false)
{
  if (sRg.generateDouble() < 0.01) {
    throw MvsError(MV_E_HANDLE);
  }
}

void
MvsCameraWrapper::close() noexcept(false)
{
  QMutexLocker locker(&_data->_mutex);

  if (sRg.generateDouble() < 0.01) {
    throw MvsError(MV_E_HANDLE);
  }
  _data->_handle = nullptr;
}

bool
MvsCameraWrapper::is_connected() noexcept(false)
{
  QMutexLocker locker(&_data->_mutex);

  if (sRg.generateDouble() < 0.01) {
    throw MvsError(MV_E_HANDLE);
  }
  return _data->_handle;
}

cv::Mat
MvsCameraWrapper::snap_cvmat(uint timeout, bool colored) noexcept(false)
{
  QMutexLocker locker(&_data->_mutex);

  if (!_data->_handle)
    throw MvsError(MV_E_CALLORDER);

  if (sRg.generateDouble() < 0.01) {
    throw MvsError(MV_E_NODATA);
  }

  if (colored) {
    cv::Mat img(1024, 1024, CV_8UC3);
    cv::circle(img, { 512, 512 }, 200, { 0, 0, 255 }, 5);
    return img;

  } else {
    cv::Mat img(1024, 1024, CV_8UC1);
    cv::rectangle(img, { 100, 100 }, { 800, 800 }, { 255, 0, 0 }, 5);
    return img;
  }
}

QImage
MvsCameraWrapper::snap_qimage(uint timeout, bool colored) noexcept(false)
{
  QMutexLocker locker(&_data->_mutex);

  if (!_data->_handle)
    throw MvsError(MV_E_CALLORDER);

  QImage image({ 800, 800 },
               colored ? QImage::Format_RGB888 : QImage::Format_Grayscale8);
  QPainter painter(&image);
  auto font = painter.font();
  font.setPixelSize(50);
  painter.setFont(font);
  painter.drawText(300, 300, "QImage 测试图片");
  painter.drawText(50, 400, name());
  painter.drawText(100, 500, QString::number(++_data->_frameCounter));
  return image;
}

void
MvsCameraWrapper::save_feature(const QString& path) noexcept(false)
{}

void
MvsCameraWrapper::load_feature(const QString& path) noexcept(false)
{}

MvsCameraWrapper::Data::~Data() {}

#endif

// =========================================================================
// MvsError
// =========================================================================

const QHash<int, const char*> MvsError::kCodeTable{
  { MV_E_HANDLE, "错误或无效的句柄" },
  { MV_E_SUPPORT, "不支持的功能" },
  { MV_E_BUFOVER, "缓存已满" },
  { MV_E_CALLORDER, "函数调用顺序有误" },
  { MV_E_PARAMETER, "错误的参数" },
  { MV_E_RESOURCE, "资源申请失败" },
  { MV_E_NODATA, "无数据" },
  { MV_E_PRECONDITION, "前置条件有误，或运行环境已发生变化" },
  { MV_E_VERSION, "版本不匹配" },
  { MV_E_NOENOUGH_BUF, "传入的内存空间不足" },
  { MV_E_ABNORMAL_IMAGE, "异常图像，可能是丢包导致图像不完整" },
  { MV_E_LOAD_LIBRARY, "动态导入DLL失败" },
  { MV_E_NOOUTBUF, "没有可输出的缓存" },
  { MV_E_ENCRYPT, "加密错误" },
  { MV_E_UNKNOW, "未知的错误" },
  { MV_E_GC_GENERIC, "通用错误" },
  { MV_E_GC_ARGUMENT, "参数非法" },
  { MV_E_GC_RANGE, "值超出范围" },
  { MV_E_GC_PROPERTY, "属性" },
  { MV_E_GC_RUNTIME, "运行环境有问题" },
  { MV_E_GC_LOGICAL, "逻辑错误" },
  { MV_E_GC_ACCESS, "节点访问条件有误" },
  { MV_E_GC_TIMEOUT, "超时" },
  { MV_E_GC_DYNAMICCAST, "转换异常" },
  { MV_E_GC_UNKNOW, "GenICam未知错误" },
  { MV_E_NOT_IMPLEMENTED, "命令不被设备支持" },
  { MV_E_INVALID_ADDRESS, "访问的目标地址不存在" },
  { MV_E_WRITE_PROTECT, "目标地址不可写" },
  { MV_E_ACCESS_DENIED, "设备无访问权限" },
  { MV_E_BUSY, "设备忙，或网络断开" },
  { MV_E_PACKET, "网络包数据错误" },
  { MV_E_NETER, "网络相关错误" },
  { MV_E_IP_CONFLICT, "设备IP冲突" },
  { MV_E_USB_READ, "读USB出错" },
  { MV_E_USB_WRITE, "写USB出错" },
  { MV_E_USB_DEVICE, "设备异常" },
  { MV_E_USB_GENICAM, "GenICam相关错误" },
  { MV_E_USB_BANDWIDTH, "带宽不足，该错误码新增" },
  { MV_E_USB_DRIVER, "驱动不匹配或者未装驱动" },
  { MV_E_USB_UNKNOW, "USB未知的错误" },
  { MV_E_UPG_FILE_MISMATCH, "升级固件不匹配" },
  { MV_E_UPG_LANGUSGE_MISMATCH, "升级固件语言不匹配" },
  { MV_E_UPG_CONFLICT, "升级冲突" },
  { MV_E_UPG_INNER_ERR, "升级时相机内部出现错误" },
  { MV_E_UPG_UNKNOW, "升级时未知错误" },
  { MV_EXCEPTION_DEV_DISCONNECT, "设备断开连接" },
  { MV_EXCEPTION_VERSION_CHECK, "SDK与驱动版本不匹配" },
};

QString
MvsError::translate() const
{
  auto trans = kCodeTable.find(_code);
  if (trans == kCodeTable.end())
    return "意料之外的错误代码";
  return trans.value();
}

QString
MvsError::repr() const noexcept
{
  return QString(what()) + '(' + QString::number(_code, 16) +
         "): " + translate();
}

}
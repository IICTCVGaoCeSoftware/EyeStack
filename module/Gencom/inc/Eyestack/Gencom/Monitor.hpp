#pragma once

#include "Eyestack/Design/Monitor.hpp"
#include "Eyestack/Framework.hpp"
#include "Eyestack/Gencom/base.hpp"

namespace Eyestack::Gencom {

// ==========================================================================
// Monitor
// ==========================================================================

/**
 * @brief Design::Monitor 的包装组件，提供了最基本的图像输出功能。
 */
class EYESTACK_GENCOM_EXPORT Monitor
  : public QObject
  , private GinitRequired
{
  Q_OBJECT

  using _T = Monitor;
  using _S = QObject;

public:
  class SubUi;

public:
  Monitor(Framework::Worker& worker)
    : _S(&worker)
  {}

public:
  /**
   * @brief 获取所处的工作对象
   */
  Framework::Worker& worker()
  {
    return *dynamic_cast<Framework::Worker*>(_S::parent());
  }

  /**
   * @brief 显示 QImage 图像
   */
  void display(const QImage& img)
  {
    if (_monitored)
      emit s_displayQImage(img);
  }

  /**
   * @brief 显示 OpenCV 图像
   */
  void display(const cv::Mat& mat)
  {
    if (_monitored)
      emit s_displayCvMat(mat);
  }

  /**
   * @brief 获取监视是否启用
   */
  bool monitored() { return _monitored; }

signals:
  /**
   * @brief 监视状态改变时发射此信号
   * @param flag 监视是否启用
   */
  void s_monitoredUpdated(bool flag, QPrivateSignal = QPrivateSignal());

  /**
   * @brief QImage 图像显示信号
   */
  void s_displayQImage(QImage img, QPrivateSignal = QPrivateSignal());

  /**
   * @brief OpenCV 图像显示信号
   */
  void s_displayCvMat(::cv::Mat mat, QPrivateSignal = QPrivateSignal());

public slots:
  /**
   * @brief 设置监视是否启用
   * @param flag 启用为 true，否则为 false
   */
  void setMonitored(bool flag)
  {
    _monitored = flag;
    emit s_monitoredUpdated(flag);
  }

private:
  bool _monitored{ false };
};

// ==========================================================================
// Monitor::SubUi
// ==========================================================================

/**
 * @brief Monitor 内置的界面类
 */
class EYESTACK_GENCOM_EXPORT Monitor::SubUi
  : public Framework::MainWindow::SubUi::MdiAction
{
  Q_OBJECT

  using _T = SubUi;
  using _S = Framework::MainWindow::SubUi::MdiAction;

public:
  SubUi(Monitor& com,
        const QString& name,
        const QIcon& icon = Asset::faicon("tv-solid"));

private:
  Monitor& _com;
  Design::Monitor _monitor;
};

}

#pragma once

#include "Eyestack/Design/Monitor.hpp"
#include "Eyestack/Design/QuadrangleSelector.hpp"
#include "Eyestack/Framework.hpp"
#include "Eyestack/Gencom/base.hpp"

namespace Eyestack::Gencom {

/**
 * @brief OpenCV 透视变换的组件包装类
 */
class EYESTACK_GENCOM_EXPORT WarpPerspective
  : public QObject
  , private GinitRequired
{
  Q_OBJECT

  using _T = WarpPerspective;
  using _S = QObject;

public:
  class SubUi;
  class Config;

public:
  /**
   * @brief 构造函数
   *
   * @param worker 所处的工作对象
   */
  WarpPerspective(Framework::Worker& worker)
    : _S(&worker)
    , _trans(cv::Matx33f::ones())
    , _dsize(1, 1)
  {}

public:
  /**
   * @brief 对图像进行仿射变换处理
   * @param src 源图片
   * @return 处理后图片
   */
  cv::Mat process(const cv::Mat& src);

public:
  /**
   * @brief 获取所处的工作对象
   */
  Framework::Worker& worker()
  {
    return *dynamic_cast<Framework::Worker*>(_S::parent());
  }

  const cv::Matx33f& trans() { return _trans; }

  const cv::Size& dsize() { return _dsize; }

  bool monitored() { return _monitored; }

signals:
  /**
   * @brief 变换矩阵更新时发射此信号
   */
  void s_transUpdated(QPrivateSignal = QPrivateSignal());

  /**
   * @brief 输出尺寸更新是发射此信号
   */
  void s_dsizeUpdated(QPrivateSignal = QPrivateSignal());

  /**
   * @brief 监视状态更新是发射此信号
   * @param val 是否启用
   */
  void s_monitoredUpdated(bool val, QPrivateSignal = QPrivateSignal());

  /**
   * @brief 当监视启用时，算法每处理一张图像就会发射此信号
   * @param img 处理后的图像
   */
  void s_refresh(::cv::Mat img, QPrivateSignal = QPrivateSignal());

public slots:
  void setTrans(const cv::Matx33f& trans)
  {
    _trans = trans;
    emit s_transUpdated();
  }

  void setDsize(const cv::Size& dsize)
  {
    _dsize = dsize;
    emit s_dsizeUpdated();
  }

  void setMonitored(bool val)
  {
    _monitored = val;
    emit s_monitoredUpdated(val);
  }

private:
  cv::Matx33f _trans;
  cv::Size _dsize;
  bool _monitored{ false };

private:
  using _S::setParent;
};

class WarpPerspective::SubUi : public Framework::MainWindow::SubUi::MdiAction
{
  Q_OBJECT

  using _T = SubUi;
  using _S = Framework::MainWindow::SubUi::MdiAction;

public:
  /**
   * @brief 获取输入图像回调
   */
  std::function<cv::Mat()> _getInput = []() { return cv::Mat(); };

public:
  SubUi(WarpPerspective& self,
        const QString& name = QString(),
        const QIcon& icon = Asset::faicon("cube-solid"));

private:
  WarpPerspective& _self;
  cv::Matx33f _trans;
  cv::Size _dsize;

  QWidget _widget;
  QHBoxLayout _mainLayout;

  QWidget _leftWidget;
  QStackedLayout _leftLayout;
  Eyestack::Design::QuadrangleSelector _inputBench;
  Eyestack::Design::ImageBench _outputBench;

  QVBoxLayout _rightLayout;
  QPushButton _inputButton, _outputButton;
  QPushButton _transButton;
  QSpinBox _widthSpin, _heightSpin;
  QPushButton _getButton, _setButton;

private slots:
  void enable_editing();
  void disable_editing();
  void calc_output();
};

class WarpPerspective::Config : public Framework::Configurer::Config
{
public:
  Config(WarpPerspective& self)
    : _self(self)
  {}

private:
  WarpPerspective& _self;

  // Config interface
public:
  virtual void reset_config() noexcept override;
  virtual void dump_config(QDataStream& ds) noexcept(false) override;
  virtual void load_config(QDataStream& ds) noexcept(false) override;
};

}

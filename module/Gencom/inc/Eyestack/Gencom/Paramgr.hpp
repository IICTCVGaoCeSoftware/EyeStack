#pragma once

#include "Eyestack/Design/Monitor.hpp"
#include "Eyestack/Design/Paramgr.hpp"
#include "Eyestack/Framework.hpp"

namespace Eyestack::Algocom {

class ParamgrUi;

/**
 * @brief 参数管理器算法组件类。被设计用于通过继承机制方便算法组件的开发，如果一
 * 个算法组件类在配置期仅需要编辑固定数量的不同类型的先验参数，在运行期仅需要一个
 * 监视器观察算法结果，则可以使用该类简化组件的编写。
 */
class EYESTACK_ALGOCOM_EXPORT Paramgr
  : public QObject
  , public Framework::Config
{
  Q_OBJECT

public:
  using Bool = Eyestack::Design::paramgr::Bool;
  using Int = Eyestack::Design::paramgr::Int;
  using Double = Eyestack::Design::paramgr::Double;

public:
  /**
   * @brief 是否启用监视
   */
  bool monitored() { return _monitored; }

  ParamgrUi* new_ui();

public slots:
  /**
   * @brief 设置监视是否启用
   * @param val 启用为 true，禁用为 false
   */
  void setMonitored(bool val)
  {
    _monitored = val;
    emit monitoredUpdated(val);
  }

protected:
  Design::Paramgr::Table _paramTable;

protected:
  /**
   * @brief 刷新监视器画面
   */
  void refresh_monitor(cv::Mat mat)
  {
    emit refreshMonitor(Base::util::cvmat_to_qimage(std::move(mat)));
  }

  // Component interface
public:
  virtual void reset_config() override;
  virtual void dump_config(QDataStream& ds) override;
  virtual void load_config(QDataStream& ds) override;

private:
  bool _monitored{ false };

signals:
  void refreshMonitor(QImage frame);
  void monitoredUpdated(bool val);

private:
  friend class ParamgrUi;
};

class ParamgrUi : public Framework::MdiSubUi
{
public:
  ParamgrUi(Paramgr& com);

private:
  Design::Paramgr _configUi;
  Design::Monitor _runningUi;

  // MdiSubUi interface
public:
  virtual QWidget* config_ui() override { return &_configUi; }
  virtual void switch_conftime() override { _configUi.setEnabled(true); }
  virtual QWidget* running_ui() override { return &_runningUi; }
  virtual void switch_runtime() override { _configUi.setEnabled(false); }
};

}

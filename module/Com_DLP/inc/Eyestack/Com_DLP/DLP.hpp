#pragma once

#include "Eyestack/Framework/Workctrl.hpp"
#include "dlpconfigui.h"
#include "ginit.hpp"

namespace Eyestack::Com_DLP {

// ==========================================================================
// MvsCamera
// ==========================================================================

class EYESTACK_COM_DLP_EXPORT DLP final
  : public QObject
  , private GinitRequired
{
  Q_OBJECT

  using _T = DLP;
  using _S = QObject;

public:
  class SubUi;

public:
  /**
   * @brief 构造函数
   * @param worker 所处的工作对象
   */
  DLP(Framework::Worker& worker)
    : _S(&worker)
  {}

public:
  /**
   * @threadsafe
   * @brief 获取所处的工作对象
   */
  Framework::Worker& worker()
  {
    auto worker = dynamic_cast<Framework::Worker*>(_S::parent());
    Q_ASSERT(worker);
    return *worker;
  }
};

// ==========================================================================
// DLP::SubUi
// ==========================================================================

class EYESTACK_COM_DLP_EXPORT DLP::SubUi
  : public Framework::MainWindow::SubUi::MdiAction
{
  Q_OBJECT

  using _T = DLP::SubUi;
  using _S = Framework::MainWindow::SubUi::MdiAction;

public:
  SubUi(DLP& com,
        const QString& name,
        const QIcon& icon = Asset::faicon("camera-solid"));

private:
  DLPConfigUi _configUi;
};

}

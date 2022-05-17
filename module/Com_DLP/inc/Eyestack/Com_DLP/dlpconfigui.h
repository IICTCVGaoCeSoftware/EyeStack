#pragma once

// 加上才能识别EyestackNamespace
#include "Eyestack/Framework.hpp"
#include "ginit.hpp"

namespace Eyestack::Com_DLP {

namespace Ui {
class DLPConfigUi;
}

class EYESTACK_COM_DLP_EXPORT DLPConfigUi
  : public QWidget
  , private GinitRequired
{
  Q_OBJECT

  using _T = DLPConfigUi;
  using _S = QWidget;

public:
  DLPConfigUi(QWidget* parent = nullptr);
  ~DLPConfigUi();

private:
  Ui::DLPConfigUi* ui;
};
}

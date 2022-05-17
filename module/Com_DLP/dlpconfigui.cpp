#include "Eyestack/Com_DLP/dlpconfigui.h"
#include "ui_dlpconfigui.h"

namespace Eyestack::Com_DLP {

DLPConfigUi::DLPConfigUi(QWidget* parent)
  : _S(parent)
  , ui(new Ui::DLPConfigUi())
{
  ui->setupUi(this);
}

DLPConfigUi::~DLPConfigUi()
{
  delete ui;
}

}

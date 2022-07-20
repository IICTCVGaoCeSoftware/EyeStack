#include "Eyestack/Com_DLP/DlpcConfigui.h"
#include "ui_dlpconfigui.h"
#include <iostream>

namespace Eyestack::Com_DLP {

DLPConfigUi::DLPConfigUi(QWidget* parent)
  : _S(parent)
  , _ui(new Ui::DLPConfigUi())
{
  _ui->setupUi(this);
  setupUi();
}

DLPConfigUi::~DLPConfigUi()
{
  delete _ui;
}

void
DLPConfigUi::setupUi()
{
}

void
DLPConfigUi::on_ProgramButton_clicked()
{
}

void
DLPConfigUi::on__selectPatternSet_currentIndexChanged(int index)
{
}

void
DLPConfigUi::on_ImageButton_clicked()
{
}

void
DLPConfigUi::on_RunButton_clicked()
{
}

void
DLPConfigUi::on_StopButton_clicked()
{
}

void
DLPConfigUi::on_pushButton_clicked()
{
}

void
DLPConfigUi::on_DeleteButton_clicked()
{
}

void
DLPConfigUi::on_PatternSet_currentRowChanged(int currentRow)
{
}

void
DLPConfigUi::on_DumpPatternSet_clicked()
{
}

void
DLPConfigUi::on_ApplyButton_clicked()
{
}

void
DLPConfigUi::on_ChessShow_clicked()
{}
}

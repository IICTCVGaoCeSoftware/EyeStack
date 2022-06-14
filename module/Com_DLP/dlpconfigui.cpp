#include "Eyestack/Com_DLP/dlpconfigui.h"
#include "ui_dlpconfigui.h"
#include <iostream>

namespace Eyestack::Com_DLP {

DLPConfigUi::DLPConfigUi(QWidget* parent)
  : _S(parent)
  , _ui(new Ui::DLPConfigUi())
{
  _ui->setupUi(this);
  setupUi();
  _ui->groupBox->setAlignment(Qt::AlignHCenter);

  _fore = new QButtonGroup(this);
  _fore->addButton(_ui->radioButton_3, 1);
  _fore->addButton(_ui->radioButton_4, 2);
  _fore->addButton(_ui->radioButton_5, 3);
  _fore->addButton(_ui->radioButton_6, 4);
  _fore->addButton(_ui->radioButton_7, 5);
  _fore->addButton(_ui->radioButton_8, 6);
  _fore->addButton(_ui->radioButton_9, 7);
  _fore->addButton(_ui->radioButton_10, 8);
  _back = new QButtonGroup(this);
  _back->addButton(_ui->radioButton_11, 11);
  _back->addButton(_ui->radioButton_12, 12);
  _back->addButton(_ui->radioButton_13, 13);
  _back->addButton(_ui->radioButton_14, 14);
  _back->addButton(_ui->radioButton_15, 15);
  _back->addButton(_ui->radioButton_16, 16);
  _back->addButton(_ui->radioButton_17, 17);
  _back->addButton(_ui->radioButton_18, 18);

  connect(_fore, SIGNAL(buttonClicked(int)), this, SLOT(handleRadioGroup(int)));
  connect(_back, SIGNAL(buttonClicked(int)), this, SLOT(handleRadioGroup(int)));
}

DLPConfigUi::~DLPConfigUi()
{
  delete _ui;
}

void
DLPConfigUi::setupUi()
{
  _ui->_selectPatternSet->setItemIcon(
    1, QIcon("D:/ICAC/DLPC-API-1.1/Asset/grip-horizontal-solid.svg"));
  _ui->_selectPatternSet->setItemIcon(
    2, QIcon("D:/ICAC/DLPC-API-1.1/Asset/grip-lines-vertical-solid.svg"));
  _ui->_selectPatternSet->setItemIcon(
    3, QIcon("D:/ICAC/DLPC-API-1.1/Asset/grip-horizontal-solid.svg"));
  _ui->_selectPatternSet->setItemIcon(
    4, QIcon("D:/ICAC/DLPC-API-1.1/Asset/grip-lines-vertical-solid.svg"));
  _ui->ConfigTable->verticalHeader()->setVisible(true);
  _ui->ConfigTable->setEditTriggers(QAbstractItemView::DoubleClicked);
  _ui->PatternSet->setEditTriggers(QAbstractItemView::NoEditTriggers);
  _ui->ImageButton->setEnabled(false);
}

void
DLPConfigUi::on_ProgramButton_clicked()
{
  /* Generate and program the pattern data to the controller flash */
  GenerateAndProgramPatternData(DLPC347X_INT_PAT_DMD_DLP3010);
  /* Load Pattern Order Table Entry from Flash */
  LoadPatternOrderTableEntryfromFlash();
  /* Generate and write pattern data to a file */
  GenerateAndWritePatternDataToFile(DLPC347X_INT_PAT_DMD_DLP3010,
                                    "pattern_data.bin");

  QMessageBox MBox;
  MBox.setWindowTitle("提示");
  MBox.setText("编译成功");
  MBox.exec();
}

void
DLPConfigUi::on__selectPatternSet_currentIndexChanged(int index)
{
  if (PatternIdx > 4) {
    PatternIdx = 5;
    return;
  }
  QString s2;

  if (_ui->_selectPatternSet->currentText() == "Add 1-bit Horizontal Pattern") {
    s2 = "1-bit : Horizontal Pattern";
    Populate1BitHorizonPatternSet(DLP3010_WIDTH, DLP3010_HEIGHT);
  } else if (_ui->_selectPatternSet->currentText() ==
             "Add 1-bit Vertical Pattern") {
    s2 = "1-bit : Vertical Pattern";
    Populate1BitVerticalPatternSet(DLP3010_WIDTH, DLP3010_HEIGHT);
  } else if (_ui->_selectPatternSet->currentText() ==
             "Add 8-bit Horizontal Pattern") {
    s2 = "8-bit : Horizontal Pattern";
    Populate8BitHorizonPatternSet(DLP3010_WIDTH, DLP3010_HEIGHT);
  } else if (_ui->_selectPatternSet->currentText() ==
             "Add 8-bit Vertical Pattern") {
    s2 = "8-bit : Vertical Pattern";
    Populate8BitVerticalPatternSet(DLP3010_WIDTH, DLP3010_HEIGHT);
  } else {
    return;
  }

  _ui->PatternSet->addItem(s2);                    //设置模型到listview上
  _ui->PatternSet->setMovement(QListView::Static); //设置数据可以自由拖动

  _ui->ImageButton->setEnabled(true);
  PatternIdx++;
  _ui->ConfigTable->setEditTriggers(QAbstractItemView::AllEditTriggers);
}

void
DLPConfigUi::on_ImageButton_clicked()
{
  QString filename = QFileDialog::getOpenFileName(
    this, "Add A Pattern", "/", "Pattern Image Files(*.bmp *.png)");
  if (filename == "")
    return;
  int i = _ui->PatternSet->currentIndex().row();
  if (i == -1)
    return;

  QListWidgetItem* a = _ui->PatternSet->currentItem();
  QString currentItem = a->text();
  if (currentItem == "1-bit : Horizontal Pattern") {
    file[0].append(filename);
    _listmodel = new QStringListModel(file[0]);
    _ui->Pattern->setModel(_listmodel);
  } else if (currentItem == "1-bit : Vertical Pattern") {
    file[1].append(filename);
    _listmodel = new QStringListModel(file[1]);
    _ui->Pattern->setModel(_listmodel);
    Populate1BitVerticalPattern(DLP3010_WIDTH, DLP3010_HEIGHT, filename);
  } else if (currentItem == "8-bit : Horizontal Pattern") {
    file[2].append(filename);
    _listmodel = new QStringListModel(file[2]);
    _ui->Pattern->setModel(_listmodel);
  } else if (currentItem == "8-bit : Vertical Pattern") {
    file[3].append(filename);
    _listmodel = new QStringListModel(file[3]);
    _ui->Pattern->setModel(_listmodel);
  }

  Populate1BitVerticalPattern(DLP3010_WIDTH, DLP3010_HEIGHT, filename);
}

void
DLPConfigUi::on_RunButton_clicked()
{
  DLPC347X_WriteOperatingModeSelect(DLPC347X_OM_SENS_INTERNAL_PATTERN);
  DLPC347X_WriteInternalPatternControl(DLPC347X_PC_START, 0xFF);
}

void
DLPConfigUi::on_StopButton_clicked()
{
  DLPC347X_WriteInternalPatternControl(DLPC347X_PC_STOP, 0);
}

void
DLPConfigUi::on_pushButton_clicked()
{
}

void
DLPConfigUi::on_DeleteButton_clicked()
{
  int i = _ui->PatternSet->currentIndex().row();
  DeletePatternSet(i);
  QListWidgetItem* item = _ui->PatternSet->takeItem(i);
  QString currentItem = item->text();
  if (currentItem == "1-bit : Horizontal Pattern") {
    file[0].clear();
  } else if (currentItem == "1-bit : Vertical Pattern") {
    file[1].clear();
  } else if (currentItem == "8-bit : Horizontal Pattern") {
    file[2].clear();
  } else if (currentItem == "8-bit : Vertical Pattern") {
    file[3].clear();
  }
  delete item;
  QVector<QListWidgetItem*> temp;
  for (int j = i + 1; j < 4; j++) {
    QListWidgetItem* item1 = _ui->PatternSet->takeItem(j);
    temp.push_back(item1);
    delete item1;
  }

  for (int j = 0; j < temp.size(); j++) {
    _ui->PatternSet->addItem(temp[j]);
  }
  PatternIdx--;
}

void
DLPConfigUi::on_PatternSet_currentRowChanged(int currentRow)
{
  QListWidgetItem* a = _ui->PatternSet->currentItem();
  QString currentItem = a->text();
  if (currentItem == "1-bit : Horizontal Pattern") {
    _listmodel = new QStringListModel(file[0]);
    _ui->Pattern->setModel(_listmodel);

  } else if (currentItem == "1-bit : Vertical Pattern") {
    _listmodel = new QStringListModel(file[1]);
    _ui->Pattern->setModel(_listmodel);
  } else if (currentItem == "8-bit : Horizontal Pattern") {
    _listmodel = new QStringListModel(file[2]);
    _ui->Pattern->setModel(_listmodel);
  } else if (currentItem == "8-bit : Vertical Pattern") {
    _listmodel = new QStringListModel(file[3]);
    _ui->Pattern->setModel(_listmodel);
  }
}

void
DLPConfigUi::setupTableWidget(QString SetName, uint8_t PatternEntryIdx, int num)
{
  QTableWidgetItem* item = new QTableWidgetItem(SetName);
  item->setTextAlignment(Qt::AlignCenter);
  item->setFlags(Qt::ItemIsEnabled);
  _ui->ConfigTable->setItem(PatternEntryIdx, 0, item);

  _pComboBox = new QComboBox();
  _pComboBox->addItem("R");
  _pComboBox->addItem("G");
  _pComboBox->addItem("B");
  _pComboBox->addItem("RGB");
  _ui->ConfigTable->setCellWidget(PatternEntryIdx, 1, _pComboBox);

  QString str = QString::number(num, 'f', 0);
  QTableWidgetItem* item4 = new QTableWidgetItem(str);
  item4->setTextAlignment(Qt::AlignCenter);
  item4->setFlags(Qt::ItemIsEnabled);
  _ui->ConfigTable->setItem(PatternEntryIdx, 2, item4);

  QTableWidgetItem* item2 = new QTableWidgetItem("250");
  item2->setTextAlignment(Qt::AlignCenter);
  // item2->setFlags(Qt::ItemIsEditable);
  _ui->ConfigTable->setItem(PatternEntryIdx, 3, item2);

  QTableWidgetItem* item1 = new QTableWidgetItem("5000");
  item1->setTextAlignment(Qt::AlignCenter);
  // item1->setFlags(Qt::ItemIsEditable);
  _ui->ConfigTable->setItem(PatternEntryIdx, 4, item1);

  QTableWidgetItem* item3 = new QTableWidgetItem("1000");
  item3->setTextAlignment(Qt::AlignCenter);
  // item3->setFlags(Qt::ItemIsEditable);
  _ui->ConfigTable->setItem(PatternEntryIdx, 5, item3);
}

void
DLPConfigUi::on_DumpPatternSet_clicked()
{

  // ========当前选中的PatternSet进行配置=================
  QListWidgetItem* a = _ui->PatternSet->currentItem();
  QString currentItem = a->text();
  uint8_t c = 0;
  if (currentItem == "1-bit : Horizontal Pattern") {
    Populate1BitHorizonPatternEntry();
    setupTableWidget(currentItem, PatternEntryIdx, file[0].size());
  } else if (currentItem == "1-bit : Vertical Pattern") {
    Populate1BitVerticalPatternEntry();
    setupTableWidget(currentItem, PatternEntryIdx, c);

  } else if (currentItem == "8-bit : Horizontal Pattern") {
    Populate8BitHorizonPatternEntry();
    setupTableWidget(currentItem, PatternEntryIdx, c);

  } else if (currentItem == "8-bit : Vertical Pattern") {
    Populate8BitVerticalPatternEntry();
    setupTableWidget(currentItem, PatternEntryIdx, c);
  }
  PatternEntryIdx++;

  // =========合并所有Pattern======================== //

  int nCnt = _ui->PatternSet->count();
  for (uint8_t i = 0; i < nCnt; ++i) {
    QListWidgetItem* pItem = _ui->PatternSet->item(i);
    QString temp = pItem->text();
    uint8_t seq = 0;
    if (pItem) {
      if (temp == "1-bit : Horizontal Pattern") {
        MergeH1PatternData(seq++);
      } else if (temp == "1-bit : Vertical Pattern") {
        MergeV1PatternData(seq++);
      } else if (temp == "8-bit : Horizontal Pattern") {
        MergeH8PatternData(seq++);
      } else if (temp == "8-bit : Vertical Pattern") {
        MergeV8PatternData(seq++);
      }
    }
  }
}

void
DLPConfigUi::on_ApplyButton_clicked()
{
  for (uint8_t i = 0; i < 4; ++i) {
    QTableWidgetItem* item = _ui->ConfigTable->item(i, 0);
    if (item) {
      QString currentItem = item->text();
      QString strValue =
        dynamic_cast<QComboBox*>(_ui->ConfigTable->cellWidget(i, 1))
          ->currentText();
      unsigned int pre = _ui->ConfigTable->item(i, 3)->text().toUInt();
      unsigned int ill = _ui->ConfigTable->item(i, 4)->text().toUInt();
      unsigned int post = _ui->ConfigTable->item(i, 5)->text().toUInt();
      if (currentItem == "1-bit : Horizontal Pattern") {
        Populate1BitHorizonPatternEntry(strValue, ill, pre, post);
      } else if (currentItem == "1-bit : Vertical Pattern") {
        Populate1BitVerticalPatternEntry(strValue, ill, pre, post);
      } else if (currentItem == "8-bit : Horizontal Pattern") {
        Populate8BitHorizonPatternEntry(strValue, ill, pre, post);
      } else if (currentItem == "8-bit : Vertical Pattern") {
        Populate8BitVerticalPatternEntry(strValue, ill, pre, post);
      }
    }
  }
}

void
DLPConfigUi::on_ChessShow_clicked()
{
  unsigned short h = _ui->spinBox->text().toUShort();
  unsigned short v = _ui->spinBox_2->text().toUShort();
  SetChessboardConfig(fore, back, h, v);
}

void
DLPConfigUi::handleRadioGroup(int id)
{
  if (id == 1) {
    fore = DLPC347X_C_BLACK;
  } else if (id == 2) {
    fore = DLPC347X_C_RED;
  } else if (id == 3) {
    fore = DLPC347X_C_GREEN;
  } else if (id == 4) {
    fore = DLPC347X_C_BLUE;
  } else if (id == 5) {
    fore = DLPC347X_C_CYAN;
  } else if (id == 6) {
    fore = DLPC347X_C_MAGENTA;
  } else if (id == 7) {
    fore = DLPC347X_C_YELLOW;
  } else if (id == 8) {
    fore = DLPC347X_C_WHITE;
  } else if (id == 11) {
    back = DLPC347X_C_BLACK;
  } else if (id == 12) {
    back = DLPC347X_C_RED;
  } else if (id == 13) {
    back = DLPC347X_C_GREEN;
  } else if (id == 14) {
    back = DLPC347X_C_BLUE;
  } else if (id == 15) {
    back = DLPC347X_C_CYAN;
  } else if (id == 16) {
    back = DLPC347X_C_MAGENTA;
  } else if (id == 17) {
    back = DLPC347X_C_YELLOW;
  } else if (id == 18) {
    back = DLPC347X_C_WHITE;
  }
}

}

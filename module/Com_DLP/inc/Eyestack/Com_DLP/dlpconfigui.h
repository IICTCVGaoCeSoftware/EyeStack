#pragma once

// 加上才能识别Eyestack Namespace
#include "Eyestack/Framework.hpp"
#include "dlpc347x_samples.hpp"
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
  void setupUi();
  /**
   * @brief 设置ConfigTable的组件
   * @param PatternSet名字
   * @param PatternEntryIdx
   * @param 图案数
   */
  void setupTableWidget(QString SetName, uint8_t PatternEntryIdx, int num);

private slots:

  void on_ProgramButton_clicked();

  void on_ImageButton_clicked();

  void on_RunButton_clicked();

  void on_StopButton_clicked();

  void on_pushButton_clicked();

  void on_DeleteButton_clicked();

  void on_PatternSet_currentRowChanged(int currentRow);

  void on_DumpPatternSet_clicked();

  void on_ApplyButton_clicked();

  void on__selectPatternSet_currentIndexChanged(int index);

  void on_ChessShow_clicked();

  void handleRadioGroup(int id);

private:
  Ui::DLPConfigUi* _ui;
  QStringList _list;
  QStringListModel* _listmodel;
  QComboBox* _pComboBox;
  QVector<QStringList> file{ 4 };
  uint16_t PatternIdx = 0;
  uint8_t PatternEntryIdx = 0;
  QButtonGroup *_fore, *_back;
  DLPC347X_Color_e fore, back;
};
}

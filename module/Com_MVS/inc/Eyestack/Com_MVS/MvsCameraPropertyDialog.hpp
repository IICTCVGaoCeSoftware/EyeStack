#pragma once

#include "Eyestack/Com_MVS/MvsCameraWrapper.hpp"

namespace Eyestack::Com_MVS {

namespace Ui {
class MvsCameraPropertyDialog;
}

class EYESTACK_COM_MVS_EXPORT MvsCameraPropertyDialog : public QDialog
{
  Q_OBJECT

public:
  MvsCameraPropertyDialog(MvsCameraWrapper& cam, QWidget* parent = nullptr);
  ~MvsCameraPropertyDialog();

private slots:
  void on_pushButton_clicked();
  void on_pushButton_2_clicked();

private:
  Ui::MvsCameraPropertyDialog* _ui;
  MvsCameraWrapper& _cam;
};

}

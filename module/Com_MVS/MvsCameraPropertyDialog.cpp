#include "Eyestack/Com_MVS/MvsCameraPropertyDialog.hpp"
#include "ui_MvsCameraPropertyDialog.h"

namespace Eyestack::Com_MVS {

MvsCameraPropertyDialog::MvsCameraPropertyDialog(MvsCameraWrapper& cam,
                                                 QWidget* parent)
  : QDialog(parent)
  , _ui(new Ui::MvsCameraPropertyDialog)
  , _cam(cam)
{
  _ui->setupUi(this);

  setWindowTitle(_cam.name());
}

MvsCameraPropertyDialog::~MvsCameraPropertyDialog()
{
  delete _ui;
}

void
MvsCameraPropertyDialog::on_pushButton_clicked()
{
  QString fileName = QFileDialog::getOpenFileName(
    this, tr("导入文件"), QString(), tr("属性流 (*.mfs)"));
  if (fileName.isEmpty())
    return;
  auto path = fileName.toLocal8Bit();

  try {
    _cam.load_feature(path);
  } catch (const MvsError& e) {
    qCritical() << e.repr();
  }
}

void
MvsCameraPropertyDialog::on_pushButton_2_clicked()
{
  QString fileName = QFileDialog::getSaveFileName(
    this, tr("导出文件"), QString(), tr("属性流 (*.mfs)"));
  if (fileName.isEmpty())
    return;
  auto path = fileName.toLocal8Bit();

  try {
    _cam.save_feature(path);
  } catch (const MvsError& e) {
    qCritical() << e.repr();
  }
}

}

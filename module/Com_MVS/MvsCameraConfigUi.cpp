#include "Eyestack/Com_MVS/MvsCameraConfigUi.hpp"
#include "Eyestack/Com_MVS/MvsCameraPropertyDialog.hpp"
#include "ui_MvsCameraConfigUi.h"

namespace Eyestack::Com_MVS {

// ==========================================================================
// MvsCameraConfigUi
// ==========================================================================

MvsCameraConfigUi::MvsCameraConfigUi(QWidget* parent)
  : _S(parent)
  , _ui(new Ui::MvsCameraConfigUi())
  , m_bOpenDevice(FALSE)
  , m_bStartGrabbing(FALSE)
  , m_nTriggerMode(MV_TRIGGER_MODE_OFF)
  , m_dExposureEdit(0)
  , m_bSoftWareTriggerCheck(FALSE)
  , m_nTriggerSource(MV_TRIGGER_SOURCE_SOFTWARE)
  , m_pSaveImageBuf(NULL)
  , m_nSaveImageBufSize(0)
{
  _ui->setupUi(this);
  memset(&m_stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));

  _ui->camlist->setModel(&_model);

  connect(_ui->configMonitor,
          &Design::Monitor::s_powerClicked,
          this,
          &_T::when_configMonitor_powerClicked);

  // 监控界面的电源键控制组件的监视状态
  connect(_ui->runningMonitor,
          &Design::Monitor::s_powerClicked,
          this,
          &_T::u_monitored);

  connect(&_timer, &QTimer::timeout, this, &_T::when_timer_timeout);

  connect(_ui->timeoutSpinBox, &QSpinBox::valueChanged, this, &_T::u_timeout);

  // 初始化状态
  update_chosen();
  update_selected();
}

MvsCameraConfigUi::~MvsCameraConfigUi()
{
  delete _ui;
}

Design::Monitor&
MvsCameraConfigUi::running_monitor()
{
  return *_ui->runningMonitor;
}

bool
MvsCameraConfigUi::colored()
{
  return _ui->coloredRadio->isChecked();
}

int
MvsCameraConfigUi::timeout()
{
  return _ui->timeoutSpinBox->value();
}

bool
MvsCameraConfigUi::monitored()
{
  return _ui->runningMonitor->get_power();
}

void
MvsCameraConfigUi::set_colored(bool colored)
{
  if (colored)
    _ui->coloredRadio->setChecked(true);
  else
    _ui->grayscaleRadio->setChecked(true);
}

void
MvsCameraConfigUi::set_timeout(int timeout)
{
  _ui->timeoutSpinBox->setValue(timeout);
}

void
MvsCameraConfigUi::set_monitored(bool monitored)
{
  _ui->runningMonitor->set_power(monitored);
}

void
MvsCameraConfigUi::update_chosen()
{
  emit _model.dataChanged(QModelIndex(), QModelIndex());
  if (_chosen) {
    _ui->runningMonitor->set_caption(_chosen->name());
    _ui->runningMonitor->setEnabled(true);

  } else {
    _ui->runningMonitor->set_caption(tr("未选中相机"));
    _ui->runningMonitor->setEnabled(false);
  }
}

void
MvsCameraConfigUi::update_selected()
{
  _ui->camlist->setCurrentIndex(_selected);

  if (_selected.isValid()) {
    _ui->configTab->setEnabled(true);

    auto& w = _wrappers[_selected.row()];
    _ui->configMonitor->set_caption(w._camera->name());
    //    _ui->configMonitor->setEnabled(true);

    if (w._grabbing)
      _timer.start(50);
    else
      _timer.stop();

    _ui->configMonitor->set_power(w._grabbing);

  } else {
    _ui->configTab->setEnabled(false);

    _ui->configMonitor->set_caption(QString());
    _ui->configMonitor->display(tr("无相机"));

    _timer.stop();
  }
}

void
MvsCameraConfigUi::SetTriggerMode()
{
  try {
    _chosen->SetEnumValue("TriggerMode", m_nTriggerMode);
  } catch (const MvsError& e) {
    QMessageBox::critical(this, tr("设置触发模式出错"), e.repr());
  }
}

void
MvsCameraConfigUi::GetTriggerMode()
{
  MVCC_ENUMVALUE stEnumValue = { 0 };
  try {
    _chosen->GetEnumValue("TriggerMode", &stEnumValue);
    m_nTriggerMode = stEnumValue.nCurValue;
    if (MV_TRIGGER_MODE_ON == m_nTriggerMode) {
      on_TriggerButton_clicked();
    } else {
      m_nTriggerMode = MV_TRIGGER_MODE_OFF;
      on_ContinueButton_clicked();
    }
  } catch (const MvsError& e) {
    QMessageBox::critical(this, tr("获取触发模式出错"), e.repr());
  }
}

void
MvsCameraConfigUi::GetExposureTime()
{
  MVCC_FLOATVALUE stFloatValue = { 0 };
  _chosen->GetFloatValue("ExposureTime", &stFloatValue);
  m_dExposureEdit = stFloatValue.fCurValue;
}

void
MvsCameraConfigUi::SetExposureTime()
{
  // ch:调节这两个曝光模式，才能让曝光时间生效
  _chosen->SetEnumValue("ExposureMode", MV_EXPOSURE_MODE_TIMED);
  _chosen->SetEnumValue("ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
  _chosen->SetFloatValue("ExposureTime", (float)m_dExposureEdit);

  QMessageBox MBox;
  MBox.setWindowTitle("提示");
  MBox.setText("设置曝光时间成功");
  MBox.exec();
}

void
MvsCameraConfigUi::GetTriggerSource()
{
  try {
    MVCC_ENUMVALUE stEnumValue = { 0 };
    _chosen->GetEnumValue("TriggerSource", &stEnumValue);
    if ((unsigned int)MV_TRIGGER_SOURCE_SOFTWARE == stEnumValue.nCurValue) {
      m_bSoftWareTriggerCheck = TRUE;
    } else {
      m_bSoftWareTriggerCheck = FALSE;
    }
  } catch (const MvsError& e) {
    QMessageBox::critical(this, tr("获取触发模式出错"), e.repr());
  }
}

void
MvsCameraConfigUi::SetTriggerSource()
{
  if (m_bSoftWareTriggerCheck) {
    m_nTriggerSource = MV_TRIGGER_SOURCE_SOFTWARE;
    _chosen->SetEnumValue("TriggerSource", m_nTriggerSource);
    _ui->SoftTriggerOnce->setEnabled(true);
    // GetDlgItem(IDC_SOFTWARE_ONCE_BUTTON )->EnableWindow(TRUE);
  } else {
    m_nTriggerSource = MV_TRIGGER_SOURCE_LINE0;
    _chosen->SetEnumValue("TriggerSource", m_nTriggerSource);
    _ui->SoftTriggerOnce->setEnabled(false);
  }
}

void
MvsCameraConfigUi::on_refreshButton_clicked()
{
  auto camlist = MvsCameraWrapper::list_all();
  for (int i = _wrappers.size(); --i != -1;) {
    auto& cam = _wrappers[i]._camera;
    for (int j = camlist.size(); --j != -1;) {
      auto& newCam = camlist[j];
      if (cam->mac_addr() == newCam->mac_addr()) {
        // HACK 不同相机的 mac_addr 可能相同？
        camlist.remove(j);
        goto SKIP_REMOVE;
      }
    }
    _wrappers.remove(i);
  SKIP_REMOVE:;
  }

  for (auto&& cam : camlist)
    _wrappers.append({ std::move(cam), false });

  emit _model.dataChanged(QModelIndex(), QModelIndex());
  _selected = QModelIndex();
  update_selected();
}

void
MvsCameraConfigUi::on_chooseButton_clicked()
{
  auto row = _ui->camlist->currentIndex().row();
  if (row < 0 || row >= _wrappers.size()) {
    QMessageBox::critical(this, tr("错误"), tr("没有选中相机"));
    return;
  }

  setChosen(_wrappers[row]._camera);
}

void
MvsCameraConfigUi::on_camlist_clicked(const QModelIndex& index)
{
  _selected = index;
  update_selected();
}

void
MvsCameraConfigUi::on_camlist_doubleClicked(const QModelIndex& index)
{
  auto row = _ui->camlist->currentIndex().row();
  if (row < 0 || row >= _wrappers.size()) {
    Base::util::popmsg::unexpected_brach(__FILE__, __LINE__, this);
    return;
  }

  auto& cam = _wrappers[row]._camera;
  try {
    if (cam->is_connected())
      try {
        cam->stop_grabbing();
        cam->close();
      } catch (const MvsError& e) {
        QMessageBox::critical(this, tr("关闭相机失败"), e.repr());
      }
    else
      try {
        cam->open();
        cam->start_grabbing();
      } catch (const MvsError& e) {
        QMessageBox::critical(this, tr("打开相机失败"), e.repr());
      }
  } catch (const MvsError& e) {
    QMessageBox::critical(this, tr("相机丢失连接"), e.repr());
  }

  emit _model.dataChanged(QModelIndex(), QModelIndex());
}

void
MvsCameraConfigUi::when_configMonitor_powerClicked(bool power)
{
  auto row = _ui->camlist->currentIndex().row();
  if (row < 0 || row >= _wrappers.size()) {
    QMessageBox::critical(this, tr("错误"), tr("没有选中相机"));
    return;
  }

  auto& w = _wrappers[row];
  if (w._grabbing)
    w._grabbing = !w._grabbing;
  else
    w._grabbing = !w._grabbing;

  update_selected();
}

void
MvsCameraConfigUi::when_timer_timeout()
{
  try {
    auto& w = _wrappers[_selected.row()];
    auto image = w._camera->snap_qimage(100, true);
    _ui->configMonitor->display(image);

  } catch (const MvsError& e) {
    _ui->configMonitor->display(e.translate());
  }
}

void
MvsCameraConfigUi::on_grayscaleRadio_clicked(bool checked)
{
  emit u_colored(false);
}

void
MvsCameraConfigUi::on_coloredRadio_clicked(bool checked)
{
  emit u_colored(true);
}

void
MvsCameraConfigUi::on_impropButton_clicked()
{
  auto row = _ui->camlist->currentIndex().row();
  if (row < 0 || row >= _wrappers.size()) {
    Base::util::popmsg::unexpected_brach(__FILE__, __LINE__, this);
    return;
  }
  auto& cam = _wrappers[row]._camera;

  QString fileName = QFileDialog::getOpenFileName(
    this, tr("导入文件"), QString(), tr("属性流 (*.mfs)"));
  if (fileName.isEmpty())
    return;

  try {
    cam->load_feature(fileName);
    QMessageBox::information(
      this, tr("导入属性树成功"), tr("成功导入文件\"%1\"").arg(fileName));
  } catch (const MvsError& e) {
    QMessageBox::critical(this, tr("导入属性树失败"), e.repr());
  }
}

void
MvsCameraConfigUi::on_expropButton_clicked()
{
  auto row = _ui->camlist->currentIndex().row();
  if (row < 0 || row >= _wrappers.size()) {
    Base::util::popmsg::unexpected_brach(__FILE__, __LINE__, this);
    return;
  }
  auto& cam = _wrappers[row]._camera;

  QString fileName = QFileDialog::getSaveFileName(
    this, tr("导出文件"), QString(), tr("属性流 (*.mfs)"));
  if (fileName.isEmpty())
    return;

  try {
    cam->save_feature(fileName);
    QMessageBox::information(
      this, tr("导出属性树成功"), tr("成功导出文件\"%1\"").arg(fileName));
  } catch (const MvsError& e) {
    QMessageBox::critical(this, tr("导出属性树失败"), e.repr());
  }
}

// ==========================================================================
// MvsCameraConfigUi::Model
// ==========================================================================

Qt::ItemFlags
MvsCameraConfigUi::Model::flags(const QModelIndex& index) const
{
  return Qt::ItemNeverHasChildren | Qt::ItemIsEnabled | Qt::ItemIsSelectable |
         Qt::ItemIsUserCheckable;
}

int
MvsCameraConfigUi::Model::rowCount(const QModelIndex& parent) const
{
  return _self._wrappers.size();
}

QVariant
MvsCameraConfigUi::Model::data(const QModelIndex& index, int role) const
{
  if (!index.isValid() || index.row() >= _self._wrappers.size())
    return QVariant();

  auto& cam = _self._wrappers[index.row()]._camera;
  switch (role) {
    case Qt::DisplayRole: {
      return cam->name();
    } break;

    case Qt::FontRole: {
      auto font = _self._ui->camlist->font();
      if (cam == _self._chosen)
        font.setBold(true);
      return font;
    } break;

    case Qt::ToolTipRole: {
      return cam->info();
    } break;

    case Qt::CheckStateRole: {
      try {
        if (cam->is_connected())
          return Qt::CheckState::Checked;
        else
          return Qt::CheckState::Unchecked;
      } catch (const MvsError& e) {
        return Qt::CheckState::PartiallyChecked;
      }
    } break;

    default:
      return QVariant();
  }
}

bool
MvsCameraConfigUi::Model::setData(const QModelIndex& index,
                                  const QVariant& value,
                                  int role)
{
  switch (role) {
    case Qt::CheckStateRole: {
      auto& cam = _self._wrappers[index.row()]._camera;
      try {
        if (cam->is_connected())
          try {
            cam->stop_grabbing();
            cam->close();
          } catch (const MvsError& e) {
            QMessageBox::critical(&_self, tr("关闭相机失败"), e.repr());
          }
        else
          try {
            cam->open();
            cam->start_grabbing();
          } catch (const MvsError& e) {
            QMessageBox::critical(&_self, tr("打开相机失败"), e.repr());
          }
      } catch (const MvsError& e) {
        QMessageBox::critical(&_self, tr("相机丢失连接"), e.repr());
      }

      return true;
    } break;

    default:
      return false;
  }
}

// TODO
void
MvsCameraConfigUi::on_ContinueButton_clicked()
{
  _ui->ContinueButton->setChecked(true);
  _ui->TriggerButton->setChecked(false);
  _ui->SoftTrigger->setEnabled(false);
  m_nTriggerMode = MV_TRIGGER_MODE_OFF;
  SetTriggerMode();
  _ui->SoftTriggerOnce->setEnabled(false);
}

void
MvsCameraConfigUi::on_TriggerButton_clicked()
{
  update();
  _ui->ContinueButton->setChecked(false);
  _ui->TriggerButton->setChecked(true);
  _ui->SoftTrigger->setEnabled(true);
  m_nTriggerMode = MV_TRIGGER_MODE_ON;
  SetTriggerMode();
  if (m_bStartGrabbing == TRUE) {
    if (TRUE == m_bSoftWareTriggerCheck) {
      _ui->SoftTriggerOnce->setEnabled(true);
    }
  }
}

void
MvsCameraConfigUi::on_SoftTrigger_clicked()
{
  update();
  SetTriggerSource();
}

void
MvsCameraConfigUi::on_SoftTriggerOnce_clicked()
{
  if (TRUE != m_bStartGrabbing) {
    return;
  }
  _chosen->CommandExecute("TriggerSoftware");
}

void
MvsCameraConfigUi::on_SaveBMPButton_clicked()
{
  bool succ = false;
  QMessageBox MBox;
  // cv::imwrite() SaveImage(MV_Image_Bmp, succ);
  if (succ) {
    MBox.setWindowTitle("提示");
    MBox.setText("保存BMP成功");
    MBox.exec();
  } else {
    MBox.setWindowTitle("提示");
    MBox.setText("保存BMP失败");
    MBox.exec();
  }
}

void
MvsCameraConfigUi::on_SetExposureTime_clicked()
{
  update();
  m_dExposureEdit = _ui->ExposureTime->text().toDouble();
  SetExposureTime();
}

void
MvsCameraConfigUi::on_GetExposureTime_clicked()
{
  GetTriggerMode();
  GetExposureTime();
  _ui->ExposureTime->setText(QString::number(m_dExposureEdit, 'f', 10));
}

bool
MvsCameraConfigUi::RemoveCustomPixelFormats(enum MvGvspPixelType enPixelFormat)
{
  int nResult = enPixelFormat & MV_GVSP_PIX_CUSTOM;
  if (MV_GVSP_PIX_CUSTOM == nResult) {
    return true;
  } else {
    return false;
  }
}

void
MvsCameraConfigUi::on_SavePNGButton_clicked()
{
  try {
    auto& w = _wrappers[_selected.row()];
    auto image = w._camera->snap_cvmat(100, true);
    QString filename = QFileDialog::getSaveFileName(this, "保存PNG", "*.png");
    QFile file(filename);
    std::vector<int> compression_params;
    compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(0);
    compression_params.push_back(cv::IMWRITE_PNG_STRATEGY);
    compression_params.push_back(cv::IMWRITE_PNG_STRATEGY_DEFAULT);
    cv::imwrite(filename.toStdString(), image, compression_params);

  } catch (const MvsError& e) {
    QMessageBox MBox;
    MBox.setWindowTitle("提示");
    MBox.setText("保存PNG失败");
    MBox.exec();
  }
}

}

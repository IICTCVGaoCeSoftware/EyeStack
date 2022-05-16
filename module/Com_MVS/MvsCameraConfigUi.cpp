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
{
  _ui->setupUi(this);
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
  _chosen->SetEnumValue("TriggerMode", m_nTriggerMode);
}

void
MvsCameraConfigUi::GetTriggerMode()
{
  MVCC_ENUMVALUE stEnumValue = { 0 };

  _chosen->GetEnumValue("TriggerMode", &stEnumValue);
  m_nTriggerMode = stEnumValue.nCurValue;
  if (MV_TRIGGER_MODE_ON == m_nTriggerMode) {
    on_TriggerButton_clicked();
  } else {
    m_nTriggerMode = MV_TRIGGER_MODE_OFF;
    on_ContinueButton_clicked();
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
}

void
MvsCameraConfigUi::GetTriggerSource()
{
  MVCC_ENUMVALUE stEnumValue = { 0 };
  _chosen->GetEnumValue("TriggerSource", &stEnumValue);
  if ((unsigned int)MV_TRIGGER_SOURCE_SOFTWARE == stEnumValue.nCurValue) {
    m_bSoftWareTriggerCheck = TRUE;
  } else {
    m_bSoftWareTriggerCheck = FALSE;
  }
}

void
MvsCameraConfigUi::SetTriggerSource()
{
  if (m_bSoftWareTriggerCheck) {
    m_nTriggerSource = MV_TRIGGER_SOURCE_SOFTWARE;
    _chosen->SetEnumValue("TriggerSource", m_nTriggerSource);
    // GetDlgItem(IDC_SOFTWARE_ONCE_BUTTON )->EnableWindow(TRUE);
  } else {
    m_nTriggerSource = MV_TRIGGER_SOURCE_LINE0;
    _chosen->SetEnumValue("TriggerSource", m_nTriggerSource);
    // GetDlgItem(IDC_SOFTWARE_ONCE_BUTTON )->EnableWindow(FALSE);
  }
}

void
MvsCameraConfigUi::SaveImage(MV_SAVE_IAMGE_TYPE enSaveImageType)
{
  MV_SAVE_IMG_TO_FILE_PARAM stSaveFileParam;
  memset(&stSaveFileParam, 0, sizeof(MV_SAVE_IMG_TO_FILE_PARAM));

  EnterCriticalSection(&m_hSaveImageMux);
  if (m_pSaveImageBuf == NULL || m_stImageInfo.enPixelType == 0) {
    LeaveCriticalSection(&m_hSaveImageMux);
    // TODOreturn MV_E_NODATA;
  }

  //  if(RemoveCustomPixelFormats(m_stImageInfo.enPixelType))
  //  {
  //    LeaveCriticalSection(&m_hSaveImageMux);
  //    return MV_E_SUPPORT;
  //  }

  stSaveFileParam.enImageType =
    enSaveImageType; // ch:需要保存的图像类型 | en:Image format to save
  stSaveFileParam.enPixelType =
    m_stImageInfo.enPixelType; // ch:相机对应的像素格式 | en:Camera pixel type
  stSaveFileParam.nWidth = m_stImageInfo.nWidth; // ch:相机对应的宽 | en:Width
  stSaveFileParam.nHeight =
    m_stImageInfo.nHeight; // ch:相机对应的高 | en:Height
  stSaveFileParam.nDataLen = m_stImageInfo.nFrameLen;
  stSaveFileParam.pData = m_pSaveImageBuf;
  stSaveFileParam.iMethodValue = 0;

  // ch:jpg图像质量范围为(50-99], png图像质量范围为[0-9] | en:jpg image nQuality
  // range is (50-99], png image nQuality range is [0-9]
  if (MV_Image_Bmp == stSaveFileParam.enImageType) {
    sprintf_s(stSaveFileParam.pImagePath,
              256,
              "Image_w%d_h%d_fn%03d.bmp",
              stSaveFileParam.nWidth,
              stSaveFileParam.nHeight,
              m_stImageInfo.nFrameNum);
  } else if (MV_Image_Jpeg == stSaveFileParam.enImageType) {
    stSaveFileParam.nQuality = 80;
    sprintf_s(stSaveFileParam.pImagePath,
              256,
              "Image_w%d_h%d_fn%03d.jpg",
              stSaveFileParam.nWidth,
              stSaveFileParam.nHeight,
              m_stImageInfo.nFrameNum);
  } else if (MV_Image_Tif == stSaveFileParam.enImageType) {
    sprintf_s(stSaveFileParam.pImagePath,
              256,
              "Image_w%d_h%d_fn%03d.tif",
              stSaveFileParam.nWidth,
              stSaveFileParam.nHeight,
              m_stImageInfo.nFrameNum);
  } else if (MV_Image_Png == stSaveFileParam.enImageType) {
    stSaveFileParam.nQuality = 8;
    sprintf_s(stSaveFileParam.pImagePath,
              256,
              "Image_w%d_h%d_fn%03d.png",
              stSaveFileParam.nWidth,
              stSaveFileParam.nHeight,
              m_stImageInfo.nFrameNum);
  }

  _chosen->SaveImageToFile(&stSaveFileParam);
  LeaveCriticalSection(&m_hSaveImageMux);
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
  //  ((CButton *)GetDlgItem(IDC_CONTINUS_MODE_RADIO))->SetCheck(TRUE);
  //  ((CButton *)GetDlgItem(IDC_TRIGGER_MODE_RADIO))->SetCheck(FALSE);
  //  ((CButton *)GetDlgItem(IDC_SOFTWARE_TRIGGER_CHECK))->EnableWindow(FALSE);
  m_nTriggerMode = MV_TRIGGER_MODE_OFF;
  SetTriggerMode();

  // GetDlgItem(IDC_SOFTWARE_ONCE_BUTTON)->EnableWindow(FALSE);
}

void
MvsCameraConfigUi::on_TriggerButton_clicked()
{
  update();
  m_nTriggerMode = MV_TRIGGER_MODE_ON;
  SetTriggerMode();
  if (m_bStartGrabbing == TRUE) {
    if (TRUE == m_bSoftWareTriggerCheck) {
      // GetDlgItem(IDC_SOFTWARE_ONCE_BUTTON )->EnableWindow(TRUE);
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
MvsCameraConfigUi::on_SaveJPGButton_clicked()
{
  SaveImage(MV_Image_Jpeg);
}

void
MvsCameraConfigUi::on_SaveBMPButton_clicked()
{
  SaveImage(MV_Image_Bmp);
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
}

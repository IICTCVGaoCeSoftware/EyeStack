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

}

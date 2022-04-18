#include "Eyestack/Gencom/VideoCapture.hpp"
#include "Eyestack/Asset.hpp"

using namespace Eyestack::Base::util;

namespace Eyestack::Gencom {

// ==========================================================================
// VideoCapture
// ==========================================================================

bool
VideoCapture::read(cv::Mat* image)
{
  QMutexLocker locker(&_mutex);

  bool ok;

  ok = _capture.read(*image);
  if (!ok) {
    // _capture.set(cv::CAP_PROP_POS_FRAMES, 0);
    // 就很奇怪，上面这么写，当数据源为单张图片时没有用，此时 read 一次就没了

    update_source();
    ok = _capture.read(*image);
  }

  if (_monitored) {
    if (ok)
      emit s_refresh(*image);
    else
      emit s_refresh(cv::Mat());
  }

  return ok;
}

bool
VideoCapture::update_source()
{
  if (_cameraIndex != -1)
    return _capture.open(_cameraIndex);

  else if (!_sourcePath.isEmpty())
    return _capture.open(_sourcePath.toStdString());

  return false;
}

// ==========================================================================
// VideoCapture::SubUi
// ==========================================================================

constexpr auto faicon = Asset::faicon;

VideoCapture::SubUi::SubUi(VideoCapture& com,
                           const QString& name,
                           const QIcon& icon)
  : _S(name, icon)
  , _com(com)
  , _fileButton(faicon("file-solid"), tr("使用文件..."))
  , _cameraButton(faicon("camera-solid"), tr("使用相机..."))
  , _customButton(faicon("link-solid"), tr("自定义数据源..."))
  , _refreshButton(faicon("sync-alt-solid"), tr("刷新画面"))
  , _monitoredCheck(tr("启用监视"))
{
  // 给 _imageLabel 添加下沉面板效果
  _imageLabel.setFrameStyle(QFrame::Panel | QFrame::Sunken);

  // 布局界面
  setWidget(&_centralWidget);
  _centralWidget.setLayout(&_mainLayout);

  _mainLayout.addLayout(&_buttonLayout);
  _buttonLayout.addWidget(&_fileButton);
  _buttonLayout.addWidget(&_cameraButton);
  _buttonLayout.addWidget(&_customButton);
  _buttonLayout.addWidget(&_refreshButton);

  _mainLayout.addWidget(&_imageLabel);
  _mainLayout.addWidget(&_monitoredCheck, 0, Qt::AlignRight);

  // 连接信号
  connect(
    &_fileButton, &QPushButton::clicked, this, &_T::when_fileButton_clicked);
  connect(&_cameraButton,
          &QPushButton::clicked,
          this,
          &_T::when_cameraButton_clicked);
  connect(&_customButton,
          &QPushButton::clicked,
          this,
          &_T::when_customButton_clicked);
  connect(&_refreshButton,
          &QPushButton::clicked,
          this,
          &_T::when_refreshButton_clicked);

  connect(
    &_monitoredCheck, &QCheckBox::clicked, &com, &VideoCapture::setMonitored);
  connect(&com,
          &VideoCapture::s_monitoredUpdated,
          &_monitoredCheck,
          &QCheckBox::setChecked);
  connect(
    &com,
    &VideoCapture::s_refresh,
    this,
    [this](cv::Mat img) { _imageLabel.display(img); },
    Qt::QueuedConnection);

  //  auto& worker = com.worker();
  //  connect(&worker, &Framework::Worker::s_started, this,
  //  &_T::disable_buttons); connect(&worker, &Framework::Worker::s_stopped,
  //  this, &_T::enable_buttons); connect(&worker, &Framework::Worker::s_paused,
  //  this, &_T::enable_buttons); connect(&worker,
  //  &Framework::Worker::s_triggerd, this, &_T::disable_buttons);

  // 初始化状态
  when_refreshButton_clicked();
}

// void
// VideoCapture::SubUi::disable_buttons()
//{
//   _fileButton.setEnabled(false);
//   _cameraButton.setEnabled(false);
//   _customButton.setEnabled(false);
//   _refreshButton.setEnabled(false);
// }

// void
// VideoCapture::SubUi::enable_buttons()
//{
//   _fileButton.setEnabled(true);
//   _cameraButton.setEnabled(true);
//   _customButton.setEnabled(true);
//   _refreshButton.setEnabled(true);
// }

void
VideoCapture::SubUi::when_fileButton_clicked()
{
  auto path = QFileDialog::getOpenFileName(this, tr("选择文件"));
  if (path.isEmpty())
    return;

  if (!_com.open(path))
    popmsg::open_file_failed(path, this);

  when_refreshButton_clicked();
}

void
VideoCapture::SubUi::when_cameraButton_clicked()
{
  bool ok;
  auto index = QInputDialog::getInt(
    this, tr("选择相机"), tr("相机编号:"), 0, 0, 15, 1, &ok);
  if (!ok)
    return;

  if (!_com.open(index))
    QMessageBox::critical(
      this, tr("错误：打开相机失败"), tr("无效的相机：%1").arg(index));

  when_refreshButton_clicked();
}

void
VideoCapture::SubUi::when_customButton_clicked()
{
  bool ok;
  auto path = QInputDialog::getText(this,
                                    tr("自定义数据源"),
                                    tr("数据源表达式："),
                                    QLineEdit::Normal,
                                    QString(),
                                    &ok);
  if (!ok)
    return;

  if (!_com.open(path))
    QMessageBox::critical(
      this, tr("错误：数据源打开失败"), tr("无效的数据源：\"%1\"").arg(path));

  when_refreshButton_clicked();
}

void
VideoCapture::SubUi::when_refreshButton_clicked()
{
  cv::Mat mat;
  if (_com.read(&mat)) {
    _imageLabel.display(mat);
  } else {
    _imageLabel.display(tr("无画面"));
  }
}

}

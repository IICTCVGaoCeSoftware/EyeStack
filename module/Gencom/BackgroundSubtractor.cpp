#include "Algocom/BackgroundSubtractor.hpp"
#include "ui_BackgroundSubtractorConfigUi.h"
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QVBoxLayout>

namespace Algocom {

// ==========================================================================
// BackgroundSubtractor
// ==========================================================================

BackgroundSubtractorConfigUi*
BackgroundSubtractor::config_ui()
{
  return new BackgroundSubtractorConfigUi(*this);
}

BackgroundSubtractorRunningUi*
BackgroundSubtractor::running_ui()
{
  return new BackgroundSubtractorRunningUi(*this);
}

void
BackgroundSubtractor::reset_config()
{
  // TODO
  //  _putoutEnabled = true;
  //  _cvalgo->clear();
}

void
BackgroundSubtractor::dump_config(QDataStream& ds)
{
  // TODO

  // opencv algorithm 对象具备直接保存配置到文件的功能，但是我这里要求的是 ds
  // 下一版本将 QDataStream 改为 DumpHelper，或者直接提供一个全局明明空间的
  // operator<< 函数？
  // 全局命名空间慎用！建议继承 QDataStream，然后在子类中提供 Helper 函数
}

void
BackgroundSubtractor::load_config(QDataStream& ds)
{
  // TODO
}

// ==========================================================================
// BackgroundSubtractorConfigUi
// ==========================================================================

BackgroundSubtractorConfigUi::BackgroundSubtractorConfigUi(
  BackgroundSubtractor& self)
  : ui(new Ui::BackgroundSubtractorConfigUi())
  , _self(self)
{
  ui->setupUi(this);
  ui->viewerLayout->addWidget(&_bgViewer);
  ui->viewerLayout->addWidget(&_maskViewer);

  //  _timer.setInterval(200);
  connect(&_timer, &QTimer::timeout, this, &_Type::trainRequested);
}

BackgroundSubtractorConfigUi::~BackgroundSubtractorConfigUi()
{
  delete ui;
}

void
BackgroundSubtractorConfigUi::train(cv::Mat frame)
{
  if (!_self._cvalgo) {
    ui->lcdNumber->display("ERROR");
    return;
  }

  cv::Mat mask;
  _self._cvalgo->apply(frame, mask, ui->learningRateSpin->value());
  _maskViewer.setImage(QImage(
    mask.data, mask.cols, mask.rows, mask.step, QImage::Format_Grayscale8));

  cv::Mat bg;
  _self._cvalgo->getBackgroundImage(bg);
  _bgViewer.setImage(
    QImage(bg.data, bg.cols, bg.rows, bg.step, QImage::Format_BGR888));

  ui->lcdNumber->display(ui->lcdNumber->intValue() + 1);
}

void
BackgroundSubtractorConfigUi::on_knnRadio_clicked()
{
  auto p = _self._cvalgo.dynamicCast<cv::BackgroundSubtractorKNN>();
  if (!p)
    p = cv::createBackgroundSubtractorKNN();
  BackgroundSubtractorKnnDialog dialog(p);
  dialog.exec();
  _self._cvalgo = p;

  // Qt 的组件都是不受控组件，他们的状态都是自行维护的，所以在 clicked
  // 槽里面处理完全没用
  // ui->knnRadio->setChecked(true);
}

void
BackgroundSubtractorConfigUi::on_mog2Radio_clicked()
{
  auto p = _self._cvalgo.dynamicCast<cv::BackgroundSubtractorMOG2>();
  if (!p)
    p = cv::createBackgroundSubtractorMOG2();
  BackgroundSubtractorMog2Dialog dialog(p);
  dialog.exec();
  _self._cvalgo = p;
}

void
BackgroundSubtractorConfigUi::on_trainButton_clicked()
{
  if (_timer.isActive()) {
    _timer.stop();
    ui->trainButton->setText(tr("训练"));
    ui->resetButton->setEnabled(true);
    ui->knnRadio->setEnabled(true);
    ui->mog2Radio->setEnabled(true);

  } else {
    _timer.start();
    ui->trainButton->setText(tr("暂停"));
    ui->resetButton->setEnabled(false);
    ui->knnRadio->setEnabled(false);
    ui->mog2Radio->setEnabled(false);
  }
}

void
BackgroundSubtractorConfigUi::on_resetButton_clicked()
{
  _self._cvalgo.reset();
  ui->knnRadio->setChecked(false);
  ui->mog2Radio->setChecked(false);
  ui->lcdNumber->display(0);
}

void
BackgroundSubtractorConfigUi::enable()
{
  setEnabled(true);
}

void
BackgroundSubtractorConfigUi::disable()
{
  setEnabled(false);
}

void
BackgroundSubtractorConfigUi::update()
{}

// ==========================================================================
// BackgroundSubtractorRunningUi
// ==========================================================================

BackgroundSubtractorRunningUi::BackgroundSubtractorRunningUi(
  BackgroundSubtractor& self)
  : _self(self)
{
  auto layout1 = new QHBoxLayout();
  auto checkBox = new QCheckBox();
  layout1->addWidget(checkBox);
  layout1->addStretch();

  auto layout2 = new QVBoxLayout();
  layout2->addLayout(layout1);
  layout2->addWidget(&_viewer);

  setLayout(layout2);

  checkBox->setText(tr("监视输出"));
  connect(checkBox, &QCheckBox::stateChanged, [this](int state) {
    if (state != Qt::CheckState::Checked) {
      _self._putoutEnabled = false;
      _viewer.setImage(QImage());
    } else {
      _self._putoutEnabled = true;
    }
  });
}

void
BackgroundSubtractorRunningUi::putout(cv::Mat out)
{
  _viewer.setImage(
    QImage(out.data, out.cols, out.rows, out.step, QImage::Format_Grayscale8));
}

void
BackgroundSubtractorRunningUi::enable()
{
  setEnabled(true);
}

void
BackgroundSubtractorRunningUi::disable()
{
  setEnabled(false);
}

void
BackgroundSubtractorRunningUi::update()
{}

// ==========================================================================
// BackgroundSubtractorKnnDialog
// ==========================================================================

#define BEGIN_FORM_LAYOUT()                                                    \
  auto layout = new QFormLayout();                                             \
  setLayout(layout)

#define ADD_ROW_BOOL(prop, desc)                                               \
  auto prop = new QCheckBox();                                                 \
  prop->setChecked(_cvalgo->get##prop());                                      \
  connect(prop, &QCheckBox::stateChanged, [prop, this](int state) {            \
    _cvalgo->set##prop(state == Qt::CheckState::Checked);                      \
  });                                                                          \
  layout->addRow(desc, prop)

#define ADD_ROW_INT(prop, desc)                                                \
  auto prop = new QSpinBox();                                                  \
  prop->setRange(INT_MIN, INT_MAX);                                            \
  prop->setValue(_cvalgo->get##prop());                                        \
  connect(prop, &QSpinBox::valueChanged, [prop, this](int value) {             \
    _cvalgo->set##prop(value);                                                 \
  });                                                                          \
  layout->addRow(desc, prop)

#define ADD_ROW_DOUBLE(prop, desc)                                             \
  auto prop = new QDoubleSpinBox();                                            \
  prop->setRange(DBL_MIN, DBL_MAX);                                            \
  prop->setValue(_cvalgo->get##prop());                                        \
  connect(prop, &QDoubleSpinBox::valueChanged, [prop, this](double value) {    \
    _cvalgo->set##prop(value);                                                 \
  });                                                                          \
  layout->addRow(desc, prop)

#define END_FORM_LAYOUT()                                                      \
  auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);                 \
  layout->addWidget(buttonBox);                                                \
  connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept)

BackgroundSubtractorKnnDialog::BackgroundSubtractorKnnDialog(
  cv::Ptr<cv::BackgroundSubtractorKNN> cvalgo)
  : _cvalgo(cvalgo)
{
  BEGIN_FORM_LAYOUT();
  ADD_ROW_BOOL(DetectShadows, "Detect Shadows");
  ADD_ROW_DOUBLE(Dist2Threshold, "Dist2 Threshold");
  ADD_ROW_INT(NSamples, "N Samples");
  ADD_ROW_DOUBLE(ShadowThreshold, "Shadow Threshold");
  ADD_ROW_INT(ShadowValue, "Shadow Value");
  ADD_ROW_INT(kNNSamples, "KNN Samples");
  END_FORM_LAYOUT();
}

// ==========================================================================
// BackgroundSubtractorMog2Dialog
// ==========================================================================

BackgroundSubtractorMog2Dialog::BackgroundSubtractorMog2Dialog(
  cv::Ptr<cv::BackgroundSubtractorMOG2> cvalgo)
  : _cvalgo(cvalgo)
{
  BEGIN_FORM_LAYOUT();
  ADD_ROW_DOUBLE(BackgroundRatio, "Background Ratio");
  ADD_ROW_DOUBLE(ComplexityReductionThreshold,
                 "complexity Reduction Threshold");
  ADD_ROW_BOOL(DetectShadows, "Detect Shadows");
  ADD_ROW_INT(History, "History");
  ADD_ROW_INT(NMixtures, "N Mixtures");
  ADD_ROW_DOUBLE(ShadowThreshold, "Shadow Threshold");
  ADD_ROW_INT(ShadowValue, "Shadow Value");
  ADD_ROW_DOUBLE(VarInit, "Var Init");
  ADD_ROW_DOUBLE(VarMax, "Var Max");
  ADD_ROW_DOUBLE(VarMin, "Var Min");
  ADD_ROW_DOUBLE(VarThreshold, "Var Threshold");
  ADD_ROW_DOUBLE(VarThresholdGen, "Var Threashold Gen");
  END_FORM_LAYOUT();
}

}

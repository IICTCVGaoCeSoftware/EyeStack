#pragma once

// 预计 OpenCV 里算法的直接包装将是 Algocom 中很大一部分组件的来源

#include <Eyestack/Design/ImageViewer.hpp>
#include <QCheckBox>
#include <QDialog>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QSpinBox>
#include <opencv2/video.hpp>

namespace Algocom {

namespace Ui {
class BackgroundSubtractorConfigUi;
}

class BackgroundSubtractor
  : public QObject
  , public Eyestack::Base::com::Component
{
  Q_OBJECT

  friend class BackgroundSubtractorConfigUi;
  friend class BackgroundSubtractorRunningUi;

signals:
  void putout(cv::Mat out);

public:
  BackgroundSubtractor()
    : _putoutEnabled(true)
    , _cvalgo()
  {}

public:
  cv::Mat process(cv::Mat frame)
  {
    cv::Mat mask;
    if (_cvalgo)
      _cvalgo->apply(frame, mask, 0);
    if (_putoutEnabled)
      emit putout(mask);
    return mask;
  }

public:
  BackgroundSubtractorConfigUi* config_ui();
  BackgroundSubtractorRunningUi* running_ui();

private:
  bool _putoutEnabled;
  cv::Ptr<cv::BackgroundSubtractor> _cvalgo;

  // Component interface
public:
  virtual uint64_t type_uuid() override { return 0xdcb0c958d80b1287; }
  virtual void reset_config() override;
  virtual void dump_config(QDataStream& ds) override;
  virtual void load_config(QDataStream& ds) override;
};

class BackgroundSubtractorConfigUi
  : public QWidget
  , public Eyestack::Base::com::MdiSubUi
{
  Q_OBJECT

  using _Type = BackgroundSubtractorConfigUi;

signals:
  void trainRequested();

public:
  BackgroundSubtractorConfigUi(BackgroundSubtractor& self);
  ~BackgroundSubtractorConfigUi();

public:
  void train(cv::Mat frame);

private:
  Ui::BackgroundSubtractorConfigUi* ui;
  BackgroundSubtractor& _self;
  Eyestack::Design::ImageViewer _bgViewer;
  Eyestack::Design::ImageViewer _maskViewer;
  QTimer _timer;

private slots:
  void on_knnRadio_clicked();
  void on_mog2Radio_clicked();
  void on_trainButton_clicked();
  void on_resetButton_clicked();

  // MdiSubUi interface
public:
  virtual void enable() override;
  virtual void disable() override;
  virtual void update() override;
};

class BackgroundSubtractorRunningUi
  : public QWidget
  , public Eyestack::Base::com::MdiSubUi
{
  Q_OBJECT

public:
  BackgroundSubtractorRunningUi(BackgroundSubtractor& self);

public slots:
  void putout(cv::Mat out);

private:
  BackgroundSubtractor& _self;
  Eyestack::Design::ImageViewer _viewer;

  // MdiSubUi interface
public:
  virtual void enable() override;
  virtual void disable() override;
  virtual void update() override;
};

class BackgroundSubtractorKnnDialog : public QDialog
{
public:
  cv::Ptr<cv::BackgroundSubtractorKNN> _cvalgo;

public:
  BackgroundSubtractorKnnDialog(cv::Ptr<cv::BackgroundSubtractorKNN> cvalgo);
};

class BackgroundSubtractorMog2Dialog : public QDialog
{
public:
  cv::Ptr<cv::BackgroundSubtractorMOG2> _cvalgo;

public:
  BackgroundSubtractorMog2Dialog(cv::Ptr<cv::BackgroundSubtractorMOG2> cvalgo);
};

}

// 明天的任务就两条：
// 1. 完成 BackgroundSubtractor 组件
// 2. 重构 MVS_Dev，并且把它移到 Devcom 下面

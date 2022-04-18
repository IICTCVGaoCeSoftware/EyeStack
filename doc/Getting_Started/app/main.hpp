/**
 * @file main.hpp
 */

#pragma once

#include "algo/Sobel.hpp"
#include <Eyestack/Framework.hpp>
#include <Eyestack/Gencom.hpp>

using namespace Getting_Started;

namespace esf = Eyestack::Framework;
namespace esg = Eyestack::Gencom;

class SobelWrapper : public QObject
{
  Q_OBJECT

  using _T = SobelWrapper;
  using _S = QObject;

public:
  Sobel _sobel;

signals:
  void s_workerStarted();
  void s_workerStopped();
  void s_workerPaused();
  void s_workerTriggerd();

public:
  SobelWrapper(esf::Worker& worker)
    : _S(&worker)
  {
    connect(&worker,
            &esf::Worker::s_started,
            this,
            &_T::s_workerStarted,
            Qt::DirectConnection);
    connect(&worker,
            &esf::Worker::s_stopped,
            this,
            &_T::s_workerStopped,
            Qt::DirectConnection);
    connect(&worker,
            &esf::Worker::s_paused,
            this,
            &_T::s_workerPaused,
            Qt::DirectConnection);
    connect(&worker,
            &esf::Worker::s_triggerd,
            this,
            &_T::s_workerTriggerd,
            Qt::DirectConnection);
  }

public slots:
  void set_border_type(int val) { _sobel.set_border_type(val); }
};

class SobelUi : public esf::MainWindow::SubUi::MdiAction
{
  Q_OBJECT

  using _T = SobelUi;
  using _S = esf::MainWindow::SubUi::MdiAction;

public:
  SobelUi(SobelWrapper& wrapper, const QString& name)
    : _S(name)
  {
    _container.setLayout(&_mainLayout);
    setWidget(&_container);

    _mainLayout.addWidget(&_comboBox);
    _comboBox.addItem("BORDER_DEFAULT", cv::BORDER_DEFAULT);
    _comboBox.addItem("BORDER_ISOLATED", cv::BORDER_ISOLATED);
    _comboBox.addItem("BORDER_REFLECT", cv::BORDER_REFLECT);
    connect(&_comboBox, &QComboBox::currentIndexChanged, this, [this](int i) {
      emit change_sobel_borderType(_comboBox.itemData(i).toInt());
    });
    connect(this,
            &_T::change_sobel_borderType,
            &wrapper,
            &SobelWrapper::set_border_type,
            Qt::QueuedConnection);

    _mainLayout.addWidget(&_paramgr);
    _paramgr.set_table(wrapper._sobel.get_param_table());
    connect(&wrapper,
            &SobelWrapper::s_workerStarted,
            this,
            &_T::disableParamgr,
            Qt::BlockingQueuedConnection);
    connect(&wrapper,
            &SobelWrapper::s_workerStopped,
            this,
            &_T::enableParamgr,
            Qt::QueuedConnection);
    connect(&wrapper,
            &SobelWrapper::s_workerPaused,
            this,
            &_T::enableParamgr,
            Qt::QueuedConnection);
    connect(&wrapper,
            &SobelWrapper::s_workerTriggerd,
            this,
            &_T::disableParamgr,
            Qt::BlockingQueuedConnection);
  }

private:
  QWidget _container;
  QVBoxLayout _mainLayout;
  Eyestack::Design::Paramgr _paramgr;
  QComboBox _comboBox;

signals:
  void change_sobel_borderType(int type);

private slots:
  void enableParamgr() { _paramgr.setEnabled(true); }
  void disableParamgr() { _paramgr.setEnabled(false); }
};

class Worker : public esf::Worker
{
public:
  esg::VideoCapture _vc{ *this };
  esg::Monitor _mo{ *this };
  SobelWrapper _sw{ *this };

  // Worker interface
protected:
  virtual void work() override;
};

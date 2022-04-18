#include "Eyestack/Gencom/WarpPerspective.hpp"

namespace Eyestack::Gencom {

// ==========================================================================
// WarpPerspective
// ==========================================================================

using namespace Base::util::serialize;

cv::Mat
WarpPerspective::process(const cv::Mat& src)
{
  cv::Mat dst;
  cv::warpPerspective(src, dst, _trans, _dsize);
  if (_monitored)
    emit s_refresh(dst);
  return dst;
}

// ======================================================================
// WarpPerspective::SubUi
// ======================================================================

WarpPerspective::SubUi::SubUi(WarpPerspective& self,
                              const QString& name,
                              const QIcon& icon)
  : _S(name, icon)
  , _self(self)
{
  setWidget(&_widget);
  _widget.setLayout(&_mainLayout);

  _mainLayout.addWidget(&_leftWidget);
  _leftWidget.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  _leftWidget.setLayout(&_leftLayout);
  {
    _leftLayout.addWidget(&_inputBench);
    _leftLayout.addWidget(&_outputBench);
    _leftLayout.setCurrentIndex(0);
  }

  _mainLayout.addLayout(&_rightLayout);
  {
    _rightLayout.addStretch();

    _rightLayout.addWidget(&_inputButton);
    _inputButton.setText(tr("刷新输入"));
    connect(&_inputButton, &QPushButton::clicked, this, [this]() {
      _leftLayout.setCurrentIndex(0);
      _inputBench.display(_getInput());
    });

    _rightLayout.addWidget(&_outputButton);
    _outputButton.setText(tr("计算输出"));
    connect(&_outputButton, &QPushButton::clicked, this, [this]() {
      _dsize = { _widthSpin.value(), _heightSpin.value() };
      _trans = cv::getPerspectiveTransform(
        _inputBench.get_quadrangle(),
        cv::Vec<cv::Point2f, 4>(
          { { 0, 0 },
            { float(_dsize.width), 0 },
            { 0, float(_dsize.height) },
            { float(_dsize.width), float(_dsize.height) } }));
      calc_output();
    });

    _rightLayout.addSpacing(20);

    _rightLayout.addWidget(&_widthSpin);
    _widthSpin.setPrefix("宽");
    _widthSpin.setSuffix("像素");
    _widthSpin.setRange(1, 4096);
    connect(&_widthSpin, &QSpinBox::valueChanged, this, [this](int val) {
      _dsize.width = val;
    });

    _rightLayout.addWidget(&_heightSpin);
    _heightSpin.setPrefix("高");
    _heightSpin.setSuffix("像素");
    _heightSpin.setRange(1, 4096);
    connect(&_heightSpin, &QSpinBox::valueChanged, this, [this](int val) {
      _dsize.height = val;
    });

    _rightLayout.addWidget(&_transButton);
    _transButton.setText(tr("仿射矩阵"));
    connect(&_transButton, &QPushButton::clicked, this, [this]() {
      QString s;
      QTextStream ts(&s);
      for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
          ts << "[" << i << "," << j << "]= " << _trans.val[i * 3 + j] << ", ";
        }
        ts << "\n";
      }
      ts.flush();
      QMessageBox::information(this, "cv::Matx33f", s);
    });

    _rightLayout.addSpacing(20);

    _rightLayout.addWidget(&_getButton);
    _getButton.setText(tr("获取参数"));
    connect(&_getButton, &QPushButton::clicked, this, [this]() {
      _trans = _self.trans();
      _dsize = _self.dsize();
      _widthSpin.setValue(_dsize.width);
      _heightSpin.setValue(_dsize.height);
      if (!_inputBench.current().isNull())
        calc_output();
    });

    _rightLayout.addWidget(&_setButton);
    _setButton.setText(tr("设置参数"));
    connect(&_setButton, &QPushButton::clicked, this, [this]() {
      auto& self = _self;
      auto& trans = _trans;
      auto& dsize = _dsize;
      QMetaObject::invokeMethod(
        &_self,
        [trans, dsize, &self]() {
          self.setDsize(dsize);
          self.setTrans(trans);
        },
        Qt::QueuedConnection);
    });

    _rightLayout.addStretch();
  }

  auto worker = &self.worker();
  connect(worker, &Framework::Worker::s_started, this, &_T::disable_editing);
  connect(worker, &Framework::Worker::s_stopped, this, &_T::enable_editing);
  connect(worker, &Framework::Worker::s_paused, this, &_T::enable_editing);
}

void
WarpPerspective::SubUi::calc_output()
{
  try {
    cv::Mat dst;
    cv::warpPerspective(
      Base::util::qimage_as_cvmat(_inputBench.current()), dst, _trans, _dsize);
    _outputBench.display(dst);
    _leftLayout.setCurrentIndex(1);

  } catch (cv::Exception& e) {
    QMessageBox::critical(this, "错误", e.what());
  }
}

void
WarpPerspective::SubUi::enable_editing()
{
  _setButton.setEnabled(true);
}

void
WarpPerspective::SubUi::disable_editing()
{
  _setButton.setEnabled(false);
}

// ==========================================================================
// WarpPerspective::Config
// ==========================================================================

void
WarpPerspective::Config::reset_config() noexcept
{
  auto& self = _self;
  QMetaObject::invokeMethod(
    &_self,
    [&self]() {
      self._trans = cv::Matx33f::ones();
      self._dsize = { 1, 1 };
      self._monitored = false;
    },
    Qt::QueuedConnection);
}

void
WarpPerspective::Config::dump_config(QDataStream& ds) noexcept(false)
{
  ds << _self._trans << _self._dsize << _self._monitored;
}

void
WarpPerspective::Config::load_config(QDataStream& ds) noexcept(false)
{
  cv::Matx33f trans;
  cv::Size dsize;
  bool monitored;

  ds >> trans >> dsize >> monitored;

  auto& self = _self;
  QMetaObject::invokeMethod(
    &_self,
    [&self, trans, dsize, monitored]() {
      self._trans = trans;
      self._dsize = dsize;
      self._monitored = monitored;
    },
    Qt::QueuedConnection);
}

}

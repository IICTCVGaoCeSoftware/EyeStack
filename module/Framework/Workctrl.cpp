#include "Eyestack/Framework/Workctrl.hpp"

namespace Eyestack::Framework {

static const QLoggingCategory sLogcat("Workctrl");

// ==========================================================================
// Worker
// ==========================================================================

void
Worker::c_start(qint64 cycle)
{
  Q_ASSERT(thread() == QThread::currentThread());

  if (_timerId > 0)
    killTimer(_timerId);

  _timerId = _S::startTimer(cycle, Qt::PreciseTimer);

  if (_timerId > 0)
    start();
}

void
Worker::c_stop()
{
  Q_ASSERT(thread() == QThread::currentThread());

  if (_timerId > 0)
    killTimer(_timerId);
  _timerId = 0;

  stop();
}

void
Worker::c_pause()
{
  Q_ASSERT(thread() == QThread::currentThread());

  if (_timerId > 0)
    killTimer(_timerId);
  _timerId = -1;

  pause();
}

void
Worker::c_trigger()
{
  Q_ASSERT(thread() == QThread::currentThread());

  if (state() == State::kStopped) {
    qInfo(sLogcat) << "工作对象" << this << "忽略了触发。";
    return;
  }

  trigger();
  do_work();

  if (state() == State::kStarted)
    start();
  else
    pause();
}

void
Worker::timerEvent(QTimerEvent* event)
{
  if (event->timerId() != _timerId) {
    _S::timerEvent(event);
    return;
  }
  do_work();
}

void
Worker::start()
{
  _state = State::kStarted;
  emit s_started();
  qInfo(sLogcat) << "工作对象" << this << "已启动。";
}

void
Worker::stop()
{
  _state = State::kStopped;
  emit s_stopped();
  qInfo(sLogcat) << "工作对象" << this << "已停止。";
}

void
Worker::pause()
{
  _state = State::kPaused;
  emit s_paused();
  qInfo(sLogcat) << "工作对象" << this << "已暂停。";
}

void
Worker::trigger()
{
  emit s_triggerd();
  qInfo(sLogcat) << "工作对象" << this << "被触发。";
}

void
Worker::do_work()
{
  try {
    _costTimer.start();
    work();
    _cost = (_cost >> 1) + (_costTimer.nsecsElapsed() >> 1);
    _cycle = (_cycle >> 1) + (_cycleTimer.restart() >> 1);
    return;

  } catch (...) {
    using namespace Base::util;
    auto exc = std::current_exception();
    qWarning(sLogcat) << "工作对象" << this << "在工作回调中抛出异常：" << exc;
    fail(exc);
  }
}

// ==========================================================================
// Workctrl
// ==========================================================================

Workctrl::~Workctrl()
{
  if (_worker != nullptr) {
    QMetaObject::invokeMethod(
      _worker,
      [this]() { _worker->moveToThread(thread()); },
      Qt::BlockingQueuedConnection);
    _worker->disconnect(this);
  }

  quit();
  wait();
}

void
Workctrl::set_worker(Worker* worker)
{
  if (_worker != nullptr) {
    QMetaObject::invokeMethod(
      _worker,
      [this]() { _worker->moveToThread(thread()); },
      Qt::BlockingQueuedConnection);
    _worker->disconnect(this);
  }

  _worker = worker;
  if (worker == nullptr)
    return;
  worker->moveToThread(this);

  connect(worker, &Worker::s_started, this, &_T::when_started);
  connect(worker, &Worker::s_stopped, this, &_T::when_stopped);
  connect(worker, &Worker::s_paused, this, &_T::when_paused);
  connect(worker, &Worker::s_failed, this, &_T::when_failed);
}

void
Workctrl::start_worker(qint64 cycle)
{
  if (!_worker)
    return;

  auto& worker = *_worker;
  QMetaObject::invokeMethod(
    _worker,
    [&worker, cycle]() { worker.c_start(cycle); },
    Qt::QueuedConnection);
}

void
Workctrl::stop_worker()
{
  if (!_worker)
    return;

  auto& worker = *_worker;
  QMetaObject::invokeMethod(
    _worker, [&worker]() { worker.c_stop(); }, Qt::QueuedConnection);
}

void
Workctrl::pause_worker()
{
  if (!_worker)
    return;

  auto& worker = *_worker;
  QMetaObject::invokeMethod(
    _worker, [&worker]() { worker.c_pause(); }, Qt::QueuedConnection);
}

void
Workctrl::trigger_worker()
{
  if (!_worker)
    return;

  auto& worker = *_worker;
  QMetaObject::invokeMethod(
    _worker, [&worker]() { worker.c_trigger(); }, Qt::QueuedConnection);
}

void
Workctrl::run()
{
  qInfo(sLogcat) << "工作线程" << this << "已启动。";
  exec();
  qInfo(sLogcat) << "工作线程" << this << "已结束。";
}

}

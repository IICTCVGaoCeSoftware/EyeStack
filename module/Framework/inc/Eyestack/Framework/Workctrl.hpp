#pragma once

#include "Eyestack/Base.hpp"

Q_DECLARE_METATYPE(::std::exception_ptr);

namespace Eyestack::Framework {

// ==========================================================================
// Worker
// ==========================================================================

/**
 * @brief 运行在工作线程中的工作对象类
 *
 * 其实现了工作控制三态模型，采用观察者模式，为上层组件化提供基础支持。
 *
 * 设计通过继承的方式使用，为子类提供了自动运行、性能监控等基本功能。
 *
 * 原TimeLooper
 * 基于 Qt 计时器和消息循环实现的自动运行工作对象
 *
 * 进入启动状态时在工作线程上启动定时器，通过工作线程的主循环实现自动运行，
 * 进入关闭状态重置主循环。
 */
class EYESTACK_FRAMEWORK_EXPORT Worker : public QObject
{
  Q_OBJECT

  using _T = Worker;
  using _S = QObject;

public:
  enum class State
  {
    kStarted,
    kStopped,
    kPaused,
  };

public:
  /**
   * @brief 获取当前工作对象状态
   */
  State state() { return _state; }

  /**
   * @brief 获取最近一次测量的工作耗时，这个方法是线程安全的。
   * @return 耗时（单位纳秒）
   */
  qint64 cost_time() { return _cost; }

  /**
   * @brief 获取最近一次测量的工作周期，这个方法是线程安全的。
   * @return 周期（单位毫秒）
   */
  qint64 cycle_time() { return _cycle; }

  // ======================================================================
  // 生命周期信号
  // ======================================================================
signals:
  /**
   * @brief 工作对象进入自动运行状态时发射此信号
   */
  void s_started(QPrivateSignal = QPrivateSignal());

  /**
   * @brief 工作对象进入停止状态时发射此信号
   */
  void s_stopped(QPrivateSignal = QPrivateSignal());

  /**
   * @brief 工作对象进入暂停状态时发射此信号
   */
  void s_paused(QPrivateSignal = QPrivateSignal());

  /**
   * @brief 工作对象被触发使发射此信号
   */
  void s_triggerd(QPrivateSignal = QPrivateSignal());

  /**
   * @brief 工作函数运行中抛出异常时发射此信号
   * @param e 异常指针
   */
  void s_failed(::std::exception_ptr e, QPrivateSignal = QPrivateSignal());

  // ======================================================================
  // 生命周期槽，用于跨线程调用
  // ======================================================================
public slots:
  /**
   * @brief 当外界请求自动运行时调用此槽，该槽在工作线程中被执行。
   * @param cycle 目标运行周期，单位毫秒
   */
  virtual void c_start(qint64 cycle);

  /**
   * @brief 当外界请求停止工作对象时调用此槽，该槽在工作线程中被执行。
   */
  virtual void c_stop();

  /**
   * @brief 当外界请求停止工作对象时调用此槽，该槽在工作线程中被执行。
   */
  virtual void c_pause();

  /**
   * @brief 当外界请求触发运行一次时调用此槽，该槽在工作线程中被执行。
   *
   * 默认实现为：当工作对象不处于停止状态时，发射触发信号，立即调用一次工作
   * 函数，然后返回之前状态（会发射相关信号）。
   */
  virtual void c_trigger();

protected:
  /**
   * @brief 工作方法（纯虚）
   */
  virtual void work() = 0;

  /**
   * @brief 工作线程抛出异常时的后退回调，在工作线程中执行。
   * 默认实现立即停止工作线程，然后发射 s_failed 信号。
   */
  virtual void fail(std::exception_ptr e)
  {
    c_stop();
    emit s_failed(e);
  }

  /**
   * @brief 配合定时器：调用一次工作函数。
   */
  virtual void timerEvent(QTimerEvent* event) override;

  // ======================================================================
  // 面向子类调用的生命周期方法
  // ======================================================================
protected:
  /**
   * @brief 在子类重载中调用，让工作对象进入启动状态
   */
  void start();

  /**
   * @brief 在子类重载中调用，让工作对象进入停止状态
   */
  void stop();

  /**
   * @brief 在子类重载中调用，让工作对象进入暂停状态
   */
  void pause();

  /**
   * @brief 在 c_trigger 重载中调用以表示工作对象被触发
   */
  void trigger();

  /**
   * @brief 执行工作，在子类重载中调用
   *
   * 执行 work 虚函数并计时，如果有异常抛出则调用 fail 虚函数。
   */
  void do_work();

private:
  State _state;
  QElapsedTimer _costTimer, _cycleTimer;
  QAtomicInteger<qint64> _cost{ 0 }, _cycle{ 0 };
  int _timerId{ 0 };
};

// ==========================================================================
// Workctrl
// ==========================================================================

/**
 * @brief 运行在主线程中的工作控制类，该类使用前需调用 ginit 全局初始化。
 */
class EYESTACK_FRAMEWORK_EXPORT Workctrl : public QThread
{
  Q_OBJECT

  using _T = Workctrl;
  using _S = QThread;

public:
  class Default;
  class Functional;

public:
  /**
   * @brief 静态全局初始化函数
   */
  static void ginit();

public:
  Workctrl(QObject* parent = nullptr)
    : _S(parent)
  {}

  virtual ~Workctrl() override;

public:
  /**
   * @brief 获取绑定的工作对象，如果没有工作对象，返回空指针。
   * @return 工作对象指针（借用语义）
   */
  Worker* get_worker() { return _worker; }

  /**
   * @brief 绑定工作对象，如果为空指针，则解绑之前的工作对象。
   * @param worker 非空工作对象指针（借用语义）
   */
  void set_worker(Worker* worker);

  /**
   * @brief 异步启动工作对象
   * @param cycle 目标周期（单位毫秒）
   */
  void start_worker(qint64 cycle);

  /**
   * @brief 异步停止工作对象
   */
  void stop_worker();

  /**
   * @brief 异步暂停工作对象
   */
  void pause_worker();

  /**
   * @brief 异步触发工作对象
   */
  void trigger_worker();

protected slots:
  /**
   * @brief 所托管的工作对象启动时的回调
   */
  virtual void when_started() = 0;

  /**
   * @brief 所托管的工作对象停止时的回调
   */
  virtual void when_stopped() = 0;

  /**
   * @brief 所托管的工作对象暂停时的回调
   */
  virtual void when_paused() = 0;

  /**
   * @brief 所托管的工作对象运行出异常时的回调
   * @param e 所抛异常
   */
  virtual void when_failed(std::exception_ptr e) = 0;

private:
  Worker* _worker{ nullptr };

  // QThread interface
protected:
  virtual void run() override;
};

class Workctrl::Default : public Workctrl
{
public:
  using Workctrl::Workctrl;

  // Workctrl interface
protected:
  virtual void when_started() override {}
  virtual void when_stopped() override {}
  virtual void when_paused() override {}
  virtual void when_failed(std::exception_ptr e) override {}
};

class Workctrl::Functional : public Workctrl
{
public:
  std::function<void()> _when_started;
  std::function<void()> _when_stopped;
  std::function<void()> _when_paused;
  std::function<void(std::exception_ptr e)> _when_failed;

public:
  using Workctrl::Workctrl;

  // Workctrl interface
protected:
  virtual void when_started() override { _when_started(); }
  virtual void when_stopped() override { _when_stopped(); }
  virtual void when_paused() override { _when_paused(); }
  virtual void when_failed(std::exception_ptr e) override { _when_failed(e); }
};

// ==========================================================================
// inlines
// ==========================================================================

inline void
Workctrl::ginit()
{
  qRegisterMetaType<::std::exception_ptr>("::std::exception_ptr");
}

}

#pragma once

namespace Eyestack::Design {

/**
 * @brief 线程安全的进度监视对话框类
 */
class EYESTACK_DESIGN_EXPORT Progressor : public QProgressDialog
{
  Q_OBJECT

  using _T = Progressor;
  using _S = QProgressDialog;

public:
  /**
   * @brief 启动子线程运行任务并监视的方便函数
   *
   * 另起线程执行 job，在 parent 上弹出模式对话框监视运行过程，等待子线程运行
   * 结束后关闭并释放对话框，而这个函数调用将立即返回。
   *
   * @param job 待执行任务
   * @param text 对话框说明文字
   * @param title 对话框标题
   * @param cancelable 是否显示取消按钮
   * @param parent 对话框的父对象
   */
  static void show(std::function<void(Progressor& pg)> job,
                   const QString& text,
                   const QString& title = QString(),
                   const QString& cancelText = QString(),
                   QWidget* parent = nullptr);

  /**
   * @brief 启动子线程运行任务并监视的方便函数
   *
   * 另起线程执行 job，在 parent 上弹出模式对话框监视运行过程，然后执行消息框
   * 的 exec，直到子线程运行结束后再返回。
   *
   * @param job 待执行任务
   * @param text 对话框说明文字
   * @param title 对话框标题
   * @param cancelText 取消按钮文本，如果为空对象则不显示按钮
   * @param parent 对话框的父对象
   */
  static void exec(std::function<void(Progressor& pg)> job,
                   const QString& text,
                   const QString& title = QString(),
                   const QString& cancelText = QString(),
                   QWidget* parent = nullptr);

public:
  using _S::_S;
  using _S::exec;
  using _S::show;

public slots:
  /**
   * @brief 设置取消标志位
   */
  void cancel()
  {
    _canceled = true;
    _S::cancel();
  }

  // ======================================================================
  // ↓ 以下方法用于子线程 ↓
  // ======================================================================

signals:
  /**
   * @brief 设定最小值，在子线程中发射
   * @param min 最小值
   * @param max 最大值
   */
  void s_setRange(int min, int max);

  /**
   * @brief 设定当前值，在子线程中发射
   * @param val 当前值
   */
  void s_setValue(int val);

public:
  /**
   * @brief 设置进度条范围，在工作对象中调用
   *
   * 将最小值最大值同时设为 0 以让进度条无限滚动
   *
   * @param min 最小值
   * @param max 最大值
   * @threadsafe
   */
  void set_range(int min = 0, int max = 100) { emit s_setRange(min, max); }

  /**
   * @brief 设置进度条当前值，在工作函数中调用
   * @param val 当前值
   * @threadsafe
   */
  void set_value(int val) { emit s_setValue(val); }

  /**
   * @brief 检查任务是否已取消，在工作对象中调用
   * @return 已取消返回 true
   * @threadsafe
   */
  bool canceled() { return _canceled; }

private:
  class Thread;

private:
  std::atomic_bool _canceled{ false };
};

class Progressor::Thread : public QThread
{
public:
  Thread(Progressor& pg, std::function<void(Progressor&)> job)
    : _pg(pg)
    , _job(job)
  {}

private:
  Progressor& _pg;
  std::function<void(Progressor&)> _job;

private:
  virtual void run() override { _job(_pg); }
};

}

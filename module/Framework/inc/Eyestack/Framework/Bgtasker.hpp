#pragma once

#include "Eyestack/Base.hpp"
#include "Eyestack/Framework/MainWindow.hpp"

namespace Eyestack::Framework {

/**
 * @brief 线程安全的任务条目类
 */
class EYESTACK_FRAMEWORK_EXPORT TaskEntry : public QWidget
{
  Q_OBJECT
  friend class QObject;
  friend class Bgtasker;

  using _T = TaskEntry;
  using _S = QWidget;

public:
  /**
   * @param title 任务标题
   * @param minimum 进度条最小值
   * @param maximum 进度条最大值，将最小值最大值同时设为 0 则让进度条无限滚动
   * @param cancelable 是否使能界面上的中断按钮
   */
  TaskEntry(const QString& title,
            int minimum = 0,
            int maximum = 0,
            bool cancelable = false);

signals:
  /**
   * @brief 任务条目被完成关闭时发射的信号
   */
  void s_finished(QPrivateSignal = QPrivateSignal());

public:
  /**
   * @brief 完成任务，关闭任务条目窗格，该方法是线程安全的
   */
  void finish()
  {
    emit s_finished();
  }

  /**
   * @brief 异步设置进度值，该方法是线程安全的
   * @param value 进度值
   */
  void set_progress(int value)
  {
    QMetaObject::invokeMethod(
      this,
      [this, value]() { _progressBar.setValue(value); },
      Qt::QueuedConnection);
  }

  /**
   * @brief 异步设置状态文本，该方法是线程安全的
   * @param text 状态文本
   */
  void set_status(const QString& text)
  {
    QMetaObject::invokeMethod(
      this,
      [this, text]() { _statusLabel.setText(text); },
      Qt::QueuedConnection);
  }

  /**
   * @brief 检测任务是否被取消，该方法是线程安全的
   * @return true 被取消，false 无取消请求
   */
  bool canceled() { return _canceled.load(std::memory_order_relaxed); }

private:
  std::atomic_bool _canceled;

  QVBoxLayout _mainLayout;
  QLabel _titleLabel;
  QProgressBar _progressBar;
  QToolButton _closeButton;
  QHBoxLayout _progressLayout;
  QLabel _statusLabel;
};

/**
 * @brief 后台任务控制服务类
 */
class EYESTACK_FRAMEWORK_EXPORT Bgtasker : protected QObject
{
  Q_OBJECT
  friend class QObject;

  using _T = Bgtasker;
  using _S = QObject;

public:
  static void ginit();

public:
  Bgtasker(MainWindow& mainWindow)
    : _mainWindow(mainWindow)
  {
    connect(this, &_T::s_post, this, &_T::when_post, Qt::QueuedConnection);
  }

  ~Bgtasker();

public:
  /**
   * @brief 发布一个任务，这个方法是线程安全的
   * @param func 任务函子，接受一个 TaskEntry& 类型的参数
   * @param title 任务标题
   * @param minimum 任务进度最小值
   * @param maximum 任务进度最大值，同时设置最小值和最大值为 0 以标识不定长任务
   * @param cancelable 是否启用取消按钮
   */
  void post(std::function<void(TaskEntry&)> func,
            const QString& title,
            int minimum = 0,
            int maximum = 0,
            bool cancelable = false)
  {
    emit s_post(func, title, minimum, maximum, cancelable);
  }

signals:
  /**
   * @brief 当有任务发布时发射此信号
   * @param func 任务函子，接受一个 TaskEntry& 类型的参数
   * @param title 任务标题
   * @param minimum 任务进度最小值
   * @param maximum 任务进度最大值，最小值和最大值同时为 0 表示不定长任务
   * @param cancelable 是否启用取消按钮
   */
  void s_post(::std::function<void(Eyestack::Framework::TaskEntry&)> func,
              QString title,
              int minimum,
              int maximum,
              bool cancelable,
              QPrivateSignal = QPrivateSignal());

private:
  MainWindow& _mainWindow;

private slots:
  void when_post(::std::function<void(TaskEntry&)> func,
                 QString title,
                 int minimum,
                 int maximum,
                 bool cancelable);
};

}

// ==========================================================================
// inlines
// ==========================================================================

Q_DECLARE_METATYPE(::std::function<void(Eyestack::Framework::TaskEntry&)>);

inline void
Eyestack::Framework::Bgtasker::ginit()
{
  qRegisterMetaType<::std::function<void(Eyestack::Framework::TaskEntry&)>>();
}

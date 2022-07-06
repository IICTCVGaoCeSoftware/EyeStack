#include "Eyestack/Framework/Bgtasker.hpp"
#include "Eyestack/Asset.hpp"
#include "Eyestack/Framework/Application.hpp"

namespace Eyestack::Framework {

constexpr auto faicon = Asset::faicon;

TaskEntry::TaskEntry(const QString& title,
                     int minimum,
                     int maximum,
                     bool cancelable)
{
  setAttribute(Qt::WA_DeleteOnClose);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  setBackgroundRole(QPalette::Window);
  setAutoFillBackground(true);

  setLayout(&_mainLayout);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  setMinimumHeight(30);
  _mainLayout.setContentsMargins(5, 3, 5, 3);
  _mainLayout.setSpacing(0);

  _titleLabel.setText(title);
  _mainLayout.addWidget(&_titleLabel);

  _mainLayout.addLayout(&_progressLayout);
  _progressLayout.setSpacing(5);

  _progressLayout.addWidget(&_progressBar);
  _progressBar.setMinimum(minimum);
  _progressBar.setMaximum(maximum);

  _progressLayout.addWidget(&_closeButton);
  _closeButton.setIcon(faicon("times-solid"));
  _closeButton.setEnabled(cancelable);
  _closeButton.setCheckable(true);
  connect(&_closeButton, &QToolButton::clicked, this, [this]() {
    _closeButton.setEnabled(false);
    _closeButton.setChecked(true);
    _statusLabel.setText(tr("尝试中断……"));
    _canceled.store(true, std::memory_order_relaxed);
  });

  _mainLayout.addWidget(&_statusLabel);
  _statusLabel.setText(tr("执行中……"));

  connect(this, &_T::s_finished, this, &_T::deleteLater, Qt::QueuedConnection);
}

Bgtasker::~Bgtasker()
{
  auto& pool = *QThreadPool::globalInstance();
  if (pool.waitForDone(100))
    return;

  QMessageBox msgBox;
  msgBox.setIcon(QMessageBox::Information);
  msgBox.addButton(tr("强制退出"), QMessageBox::AcceptRole);
  msgBox.show();

  do {
    msgBox.setText(tr("正在等待后台任务结束……\n\n当前活跃任务：%1 个")
                     .arg(pool.activeThreadCount()));
    QApplication::processEvents();
  } while (!pool.waitForDone(100) && msgBox.isVisible());
}

void
Bgtasker::when_post(::std::function<void(TaskEntry&)> func,
                    QString title,
                    int minimum,
                    int maximum,
                    bool cancelable)
{
  // 返回Qt应用程序全局线程池实例
  auto& pool = *QThreadPool::globalInstance();
  auto taskEntry = new TaskEntry(title, minimum, maximum, cancelable);
  pool.start([taskEntry, func, title]() {
    try {
      func(*taskEntry);
    } catch (...) {
      // 防止 func 抛出异常导致泄露
      auto& notifier = Application::notifier();
      notifier.notify_error(std::current_exception(),
                            tr("《%1》失败").arg(title));
    }
    taskEntry->finish();
  });
  _mainWindow._taskctrlWindow._listScroll.add_entry(taskEntry);
  _mainWindow._taskctrlWindow.show();
}

}

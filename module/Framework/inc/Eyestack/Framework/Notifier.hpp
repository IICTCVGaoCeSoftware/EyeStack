#pragma once

#include "Eyestack/Base.hpp"
#include "Eyestack/Framework/MainWindow.hpp"

Q_DECLARE_METATYPE(::std::function<QWidget*()>);

namespace Eyestack::Framework {

/**
 * @brief 通知服务模块类
 *
 * 提供线程安全的异步通知功能。目前暂时仅提供提示
 * 文本消息的功能，消息类型同 QtMsgType。设计上曾经考虑过与 Logsys 合并，
 * 之所以没有最终合并到 Logsys 是希望保留这个类为通知消息类型的拓展留出
 * 迭代空间。
 */
class EYESTACK_FRAMEWORK_EXPORT Notifier : private QObject
{
  Q_OBJECT
  friend class QObject;

  using _T = Notifier;
  using _S = QObject;

public:
  static void ginit();

signals:
  /**
   * @brief 连接这个信号来监听通用类型通知
   */
  void s_notify(::std::function<QWidget*()> note,
                int msgtype,
                QString title,
                QPrivateSignal = QPrivateSignal());

  /**
   * @brief 连接这个信号来监听文本通知
   * @param title 通知标题
   * @param text 通知内容
   * @param msgtype 通知类型，QtMsgType 枚举值
   */
  void s_notifyText(QString text,
                    int msgtype,
                    QString title,
                    QPrivateSignal = QPrivateSignal());

  /**
   * @brief 连接这个信号来监听异常通知
   * @param title 通知标题
   * @param e 异常指针对象
   */
  void s_notifyError(::std::exception_ptr e,
                     QString title,
                     QPrivateSignal = QPrivateSignal());

public:
  Notifier(MainWindow& mainWindow);

public:
  /**
   * @brief 通用类型消息通知
   * @param note 通知函数，在主线程中执行。它应该在堆上创建用于显示的 QWidget
   * 对象并返回其指针，框架负责对象的释放。
   */
  void notify(std::function<QWidget*()> note,
              QtMsgType msgtype = QtMsgType::QtInfoMsg,
              const QString& title = QString())
  {
    emit s_notify(note, msgtype, title);
  }

  /**
   * @brief 发布一则文本通知，该方法是线程安全的
   * @param text 内容
   * @param msgtype 通知类型，标识通知的严重级别，默认为 QtInfoMsg
   * @param title 标题，如果为空，则根据 msgtype 自动设置标题
   */
  void notify_text(const QString& text,
                   QtMsgType msgtype = QtMsgType::QtInfoMsg,
                   const QString& title = QString())
  {
    emit s_notifyText(text, msgtype, title);
  }

  /**
   * @brief 发布一则异常通知，该方法是线程安全的
   * @param title 通知标题
   * @param e 异常指针对象
   */
  void notify_error(::std::exception_ptr e, const QString& title)
  {
    emit s_notifyError(e, title);
  }

private:
  // 内部使用的通知条目组件模板
  class Note : public QWidget
  {
    using _T = Note;
    using _S = QWidget;

  public:
    Note(QWidget* content, QtMsgType msgtype, const QString& title);

  private:
    QVBoxLayout _mainLayout;
    QHBoxLayout _titleLayout;
    QLabel _iconLabel;
    QLabel _textLabel;
    QToolButton _closeButton;
  };

private:
  MainWindow& _mainWindow;

private slots:
  void when_notify(std::function<QWidget*()> note, int msgtype, QString title);
  void when_notifyText(QString text, int msgtype, QString title);
  void when_notifyError(::std::exception_ptr e, QString title);
};

}

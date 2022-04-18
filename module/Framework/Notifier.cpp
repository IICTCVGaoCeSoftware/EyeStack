#include "Eyestack/Framework/Notifier.hpp"

namespace Eyestack::Framework {

constexpr auto faicon = Asset::faicon;

static QScopedPointer<QPixmap> sDebugPixmap;
static QScopedPointer<QPixmap> sInfoPixmap;
static QScopedPointer<QPixmap> sWarningPixmap;
static QScopedPointer<QPixmap> sCriticalPixmap;
static QScopedPointer<QPixmap> sFatalPixmap;

void
Notifier::ginit()
{
  qRegisterMetaType<::std::function<QWidget*()>>();

  sDebugPixmap.reset(new QPixmap(faicon("bug-solid").pixmap(20, 20)));
  sInfoPixmap.reset(new QPixmap(faicon("info-circle-solid").pixmap(20, 20)));
  sWarningPixmap.reset(
    new QPixmap(faicon("exclamation-triangle-solid").pixmap(20, 20)));
  sCriticalPixmap.reset(
    new QPixmap(faicon("window-close-solid").pixmap(20, 20)));
  sFatalPixmap.reset(new QPixmap(faicon("skull-solid").pixmap(20, 20)));
}

Notifier::Notifier(MainWindow& mainWindow)
  : _S(&mainWindow)
  , _mainWindow(mainWindow)
{
  connect(
    this, &_T::s_notifyText, this, &_T::when_notifyText, Qt::QueuedConnection);
  connect(this,
          &_T::s_notifyError,
          this,
          &_T::when_notifyError,
          Qt::QueuedConnection);
  connect(this, &_T::s_notify, this, &_T::when_notify, Qt::QueuedConnection);
}

void
Notifier::when_notify(std::function<QWidget*()> note,
                      int msgtype,
                      QString title)
{
  if (title.isNull()) {
    switch (msgtype) {
      case QtDebugMsg:
        title = tr("调试");
        break;
      case QtInfoMsg:
        title = tr("信息");
        break;
      case QtWarningMsg:
        title = tr("警告");
        break;
      case QtCriticalMsg:
        title = tr("严重");
        break;
      case QtFatalMsg:
        title = tr("致命");
        break;
    }
  }
  _mainWindow._notifierWindow._listScroll.add_entry(
    new Note(note(), QtMsgType(msgtype), title));
  _mainWindow._notifierWindow.show();
}

void
Notifier::when_notifyText(QString text, int msgtype, QString title)
{
  auto label = new QLabel(text);
  label->setWordWrap(true);
  if (title.isNull()) {
    switch (msgtype) {
      case QtDebugMsg:
        title = tr("调试");
        break;
      case QtInfoMsg:
        title = tr("信息");
        break;
      case QtWarningMsg:
        title = tr("警告");
        break;
      case QtCriticalMsg:
        title = tr("严重");
        break;
      case QtFatalMsg:
        title = tr("致命");
        break;
    }
  }
  _mainWindow._notifierWindow._listScroll.add_entry(
    new Note(label, QtMsgType(msgtype), title));
  _mainWindow._notifierWindow.show();
}

void
Notifier::when_notifyError(::std::exception_ptr e, QString title)
{
  try {
    std::rethrow_exception(e);

  } catch (Base::exc::Exception& e) {
    when_notifyText(e.repr(), QtMsgType::QtCriticalMsg, title);

  } catch (std::exception& e) {
    when_notifyText(e.what(), QtMsgType::QtCriticalMsg, title);

  } catch (...) {
    when_notifyText(tr("未知错误"), QtMsgType::QtCriticalMsg, title);
  }
}

Notifier::Note::Note(QWidget* content, QtMsgType msgtype, const QString& title)
{
  setAttribute(Qt::WA_DeleteOnClose);
  setMinimumHeight(50);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  setBackgroundRole(QPalette::Window);
  setAutoFillBackground(true);

  setLayout(&_mainLayout);
  _mainLayout.addWidget(content);
  _mainLayout.addLayout(&_titleLayout);

  _titleLayout.addWidget(&_iconLabel);
  _iconLabel.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  switch (msgtype) {
    case QtDebugMsg:
      _iconLabel.setPixmap(*sDebugPixmap);
      break;
    case QtInfoMsg:
      _iconLabel.setPixmap(*sInfoPixmap);
      break;
    case QtWarningMsg:
      _iconLabel.setPixmap(*sWarningPixmap);
      break;
    case QtCriticalMsg:
      _iconLabel.setPixmap(*sCriticalPixmap);
      break;
    case QtFatalMsg:
      _iconLabel.setPixmap(*sFatalPixmap);
      break;
  }

  _titleLayout.addWidget(&_textLabel);
  _textLabel.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  _textLabel.setText(title);
  _textLabel.setEnabled(false);

  _titleLayout.addWidget(&_closeButton);
  _closeButton.setIcon(faicon("times-solid"));
  connect(&_closeButton, &QToolButton::clicked, this, &QWidget::close);
}

}

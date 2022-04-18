#pragma once

namespace Eyestack::Design {

/**
 * @brief Eyestack 及所有第三方依赖许可协议的关于对话框
 */
class EYESTACK_DESIGN_EXPORT AboutEyestack : public QDialog
{
  Q_OBJECT

  using _T = AboutEyestack;
  using _S = QDialog;

public:
  AboutEyestack(QWidget* parent = nullptr);

private:
  QVBoxLayout _mainLayout;

  QHBoxLayout _upperLayout;
  QLabel _esLogo;
  QHBoxLayout _rightLayout;
  QLabel _esInfo;
  QPushButton _aboutQt;

  QTabWidget _tabWidget;
  QTextEdit _tabFontAwesome;

private slots:
  void when_aboutQt_clicked() { QMessageBox::aboutQt(this); }
};

}

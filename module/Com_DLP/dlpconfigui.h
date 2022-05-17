#ifndef DLPCONFIGUI_H
#define DLPCONFIGUI_H

#include <QWidget>

namespace Ui {
class DLPConfigUi;
}

class DLPConfigUi : public QWidget
{
  Q_OBJECT

    public:
             explicit DLPConfigUi(QWidget *parent = nullptr);
  ~DLPConfigUi();

private:
  Ui::DLPConfigUi *ui;
};

#endif // DLPCONFIGUI_H

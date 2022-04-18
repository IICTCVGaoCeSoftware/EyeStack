#include "Eyestack/Design/Progressor.hpp"

namespace Eyestack::Design {

void
Progressor::show(std::function<void(Progressor&)> job,
                 const QString& text,
                 const QString& title,
                 const QString& cancelText,
                 QWidget* parent)
{
  auto pg = new Progressor(text, cancelText, 0, 0, parent);
  pg->setWindowTitle(title);

  auto thread = new Thread(*pg, job);
  connect(thread, &Thread::finished, pg, &_T::accept);
  connect(thread, &Thread::finished, thread, &Thread::deleteLater);
  connect(thread, &Thread::finished, pg, &Progressor::deleteLater);

  thread->start();
  pg->exec();
}

void
Progressor::exec(std::function<void(Progressor&)> job,
                 const QString& text,
                 const QString& title,
                 const QString& cancelText,
                 QWidget* parent)
{
  Progressor pg(text, cancelText, 0, 0, parent);
  pg.setWindowTitle(title);

  Thread thread(pg, job);
  connect(&thread, &Thread::finished, &pg, &_T::accept);

  thread.start();
  pg.exec();
}

}

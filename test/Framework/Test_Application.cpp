#include "Test_Application.hpp"

#include "Eyestack/Framework.hpp"

using namespace Eyestack;

BOOST_AUTO_TEST_CASE(case_1)
{
  static int argc = 0;
  Framework::Application app(argc, nullptr);

  QLabel label("test label");
  Framework::MainWindow::SubUi::MdiAction mdiAction("MDI Action");
  mdiAction.setWidget(&label);
  app.main_window().register_sub_ui(mdiAction);

  Framework::MainWindow::SubUi::MdiMenu mdiMenu("MDI-Ex Sub-UI");
  QTextEdit te1("text edit 1");
  mdiMenu.add_mdi_action("te1", te1);

  QTextEdit te2("text edit 1");
  mdiMenu.add_mdi_action("te2", te2);

  QTextEdit te3("text edit 1");
  mdiMenu.add_mdi_action("te3", te3);

  app.main_window().register_sub_ui(mdiMenu);

  //  app.workctrl().bind_worker(new Framework::Worker::TimerLooper());

  QMessageBox testctrl(QMessageBox::Question,
                       "测试控制",
                       "测试通过？",
                       QMessageBox::Yes | QMessageBox::No);
  testctrl.setModal(false);
  testctrl.show();

  app.connect(&testctrl, &QMessageBox::accepted, &app, &QApplication::quit);
  app.connect(&testctrl, &QMessageBox::rejected, [&app]() { app.exit(-1); });

  BOOST_TEST(app.exec() == 0);
}

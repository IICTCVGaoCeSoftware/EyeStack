#include "Test_MainWindow.hpp"

#define BOOST_TEST_MODULE Test_MainWindow
#include <boost/test/unit_test.hpp>

#include "Eyestack/Framework.hpp"

using namespace Eyestack;

BOOST_AUTO_TEST_CASE(case_1)
{
  static int argc = 0;
  QApplication app(argc, nullptr);

  QMessageBox testctrl(QMessageBox::Question,
                       "测试控制",
                       "测试通过？",
                       QMessageBox::Yes | QMessageBox::No);
  testctrl.setModal(false);
  testctrl.show();

  app.connect(&testctrl, &QMessageBox::accepted, &app, &QApplication::quit);
  app.connect(&testctrl, &QMessageBox::rejected, [&app]() { app.exit(-1); });

  Framework::MainWindow mainWindow;
  mainWindow.show();

  QLabel testLabel("test label");
  Framework::MainWindow::SubUi::Mdi mdiSubUi("mdiSubUi");
  mdiSubUi.setWidget(&testLabel);
  mainWindow.register_sub_ui(mdiSubUi);

  Framework::MainWindow::SubUi::MdiMenu mdiExSubUi("mdiExSubUi");
  QPushButton testPushButton1("test push button1");
  QPushButton testPushButton2("test push button2");
  mdiExSubUi.add_widget(testPushButton1, "testPushButton1");
  mdiExSubUi.add_widget(testPushButton2, "testPushButton2");
  mainWindow.register_sub_ui(mdiExSubUi);

  BOOST_TEST(app.exec() == 0);
}

#define BOOST_TEST_MODULE Test_MvsCameraConfigUi
#include <boost/test/unit_test.hpp>

#include "Eyestack/Com_MVS/MvsCameraConfigUi.hpp"
#include "Eyestack/Framework.hpp"

using namespace Eyestack;

BOOST_AUTO_TEST_CASE(test_Application)
{
  static int argc = 0;
  Framework::Application app(argc, nullptr);

  Com_MVS::ginit();

  Com_MVS::MvsCameraConfigUi ui;
  ui.show();

  BOOST_TEST(app.exec() == 0);
}

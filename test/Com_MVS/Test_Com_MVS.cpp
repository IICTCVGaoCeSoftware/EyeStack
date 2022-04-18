#define BOOST_TEST_MODULE Test_Base
#include <boost/test/unit_test.hpp>

#include "Eyestack/Com_MVS/MvsCamera.hpp"
#include "Eyestack/Framework.hpp"

using namespace Eyestack;

class Worker : public Eyestack::Framework::Worker
{
public:
  Com_MVS::MvsCamera _mc{ *this };

public:
  Worker() {}

  // Worker interface
private:
  virtual void work() override;
};

using namespace Eyestack;

BOOST_AUTO_TEST_CASE(test_Application)
{
  static int argc = 0;
  Framework::Application app(argc, nullptr);

  Com_MVS::ginit();

  auto worker = new Worker();
  app.reg_worker(worker);

  Com_MVS::MvsCamera::SubUi sui(worker->_mc, "MVS\nCamera");
  app.reg_sub_ui(sui);

  BOOST_TEST(app.exec() == 0);
}

void
Worker::work()
{
  auto img = _mc.snap();
  qDebug() << img.cols << img.rows
           << QString::fromStdString(cv::typeToString(img.type()));
}

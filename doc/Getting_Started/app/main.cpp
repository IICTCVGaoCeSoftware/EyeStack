/**
 * @file main.cpp
 */

#include "main.hpp"

int
main(int argc, char* argv[])
{
  esf::Application app(argc, argv);
  esg::ginit();

  auto worker = new Worker();
  app.reg_worker(worker);

  esg::VideoCapture::SubUi vcSubUi(worker->_vc, "视频捕获器");
  app.reg_sub_ui(vcSubUi);

  esg::Monitor::SubUi moSubUi(worker->_mo, "监视器");
  app.reg_sub_ui(moSubUi);

  SobelUi swSubUi(worker->_sw, "Sobel");
  app.reg_sub_ui(swSubUi);

  return app.exec();
}

void
Worker::work()
{
  cv::Mat mat;
  if (_vc.read(&mat))
    _mo.display(_sw._sobel.process(mat));
}

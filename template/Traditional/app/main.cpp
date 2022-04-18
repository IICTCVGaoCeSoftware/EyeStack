#include "main.hpp"
#include "version.hpp"

int
main(int argc, char* argv[])
{
  esf::Application::Manifest manifest;
  manifest.name = PROJECT_NAME;
  manifest.version = PROJECT_VERSION;

  esf::Application app(argc, argv, manifest);

  es::Gencom::ginit();

  auto worker = new Worker();

  esg::VideoCapture::Config vcConf{ worker->_vc };
  app.reg_config(vcConf);

  esg::VideoCapture::SubUi vcSubUi{ worker->_vc, "视频捕获器" };
  app.reg_sub_ui(vcSubUi);

  return app.exec();
}

void
Worker::work()
{
  cv::Mat mat;
  if (!_vc.read(&mat)) {
    auto& app = *esf::Application::instance();
    app.notifier().notify_text("无法从数据源读出图像",
                               QtMsgType::QtCriticalMsg);
    c_stop();
  }
}

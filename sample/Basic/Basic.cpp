#include "Basic.hpp"

int
main(int argc, char* argv[])
{
  Framework::Application app(argc, argv);

  Gencom::ginit();

  auto worker = new Worker();
  app.reg_worker(worker);


  Gencom::VideoCapture::SubUi vcUi(worker->vc, "视频捕获器");
  app.reg_sub_ui(vcUi);

  Gencom::WarpPerspective::SubUi wpUi(worker->wp, "仿射变换");
  app.reg_sub_ui(wpUi);
  wpUi._getInput = [worker]() -> cv::Mat {
    cv::Mat mat;
    QMetaObject::invokeMethod(
      worker,
      [&mat, worker]() { worker->vc.read(&mat); },
      Qt::BlockingQueuedConnection);
    return mat;
  };

  Gencom::WarpPerspective::Config wpConf{ worker->wp };
  app.reg_config(0x123456, wpConf);

  Gencom::Monitor::SubUi moUi(worker->mo, "运行监控");
  app.reg_sub_ui(moUi);

  app.bgtasker().post(&example_task, "任务示例", 0, 100, true);

  return app.exec();
}

Worker::Worker() {}

void
Worker::work()
{
  cv::Mat mat;
  vc.read(&mat);
  mat = wp.process(mat);
  mo.display(mat);
}

void
example_task(Framework::TaskEntry& te)
{
  int i = 0;
  while (!te.canceled() && i < 100) {
    QThread::msleep(100);
    te.set_progress(++i);
  }
  Framework::Application::notifier().notify(
    []() -> QWidget* {
      auto button = new QPushButton("再来一遍");
      QObject::connect(button, &QPushButton::clicked, []() {
        Framework::Application::bgtasker().post(
          &example_task, "新任务示例", 0, 100, true);
      });
      return button;
    },
    QtMsgType::QtInfoMsg,
    "新任务示例完成");
}

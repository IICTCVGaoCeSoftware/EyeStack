#include "Custom_UI.hpp"

using namespace Eyestack;

int
main(int argc, char* argv[])
{
  Framework::Application app(argc, argv);

  MyMdiSubUi ui;
  app._mainWindow.register_mdi_sub_ui(ui, "MyMdiSubUi");

  return app.exec();
}

MyMdiSubUi::MyMdiSubUi()
  : _hasConfigUi(true)
  , _hasRunningUi(true)
  , _configUi(QObject::tr("配置期界面"))
  , _runningUi(QObject::tr("运行期界面"))
{
  change_config_ui(&_configUi);
  change_running_ui(&_runningUi);

  QObject::connect(
    &_configUi, &QPushButton::clicked, [this]() { change_config_ui(nullptr); });

  QObject::connect(&_runningUi, &QPushButton::clicked, [this]() {
    change_running_ui(nullptr);
  });
}

void
MyMdiSubUi::switch_conftime()
{
  _configUi.setEnabled(true);
  _runningUi.setEnabled(false);
}

void
MyMdiSubUi::switch_runtime()
{
  _runningUi.setEnabled(true);
  _configUi.setEnabled(false);
}

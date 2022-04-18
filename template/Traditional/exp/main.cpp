#include "main.hpp"
#include "algo/Adder.hpp"
#include <Eyestack/Framework.hpp>

namespace esb = Eyestack::Base;
namespace esd = Eyestack::Design;
namespace esf = Eyestack::Framework;

int
main(int argc, char* argv[])
{
  // 创建 Eyestack 应用对象
  esf::Application app(argc, argv);

  // 创建算法对象
  Adder::IntAdder adder;

  // 创建参数配置器界面控件并把算法对象的参数表绑定上去
  esd::Paramgr paramgr;
  paramgr.set_table(adder.param_table());

  // 运行参数管理器控件直到用户关闭组件
  esb::util::exec_widget(paramgr);

  // 调用算法对象上的算法，将结果打印日志
  qDebug() << adder.calc(123);
}

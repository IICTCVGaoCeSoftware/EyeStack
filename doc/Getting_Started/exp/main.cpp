#include "main.hpp"
#include "algo/Sobel.hpp"
#include "version.hpp"
#include <Eyestack/Design/ImageBench.hpp>
#include <Eyestack/Framework.hpp>

using namespace Getting_Started;

namespace esb = Eyestack::Base;
namespace esd = Eyestack::Design;
namespace esf = Eyestack::Framework;

int
main(int argc, char* argv[])
{
  // 创建 Eyestack 应用对象
  esf::Application app(argc, argv);

  // 创建算法对象
  Sobel sobel;

  // 创建参数配置器界面控件并把算法对象的参数表绑定上去
  esd::Paramgr paramgr;
  paramgr.set_table(sobel.get_param_table());

  // 运行参数管理器控件直到用户关闭组件
  esb::util::exec_widget(paramgr);

  // 配置算法参数
  sobel.set_border_type(cv::BORDER_DEFAULT);

  // 读入图片
  auto img = cv::imread(PROJECT_SOURCE_DIR "/exp/photo.png");

  // 调用算法处理图像
  img = sobel.process(img);

  // 在屏幕上显示结果
  esd::ImageBench imageBench;
  imageBench.display(img);
  esb::util::exec_widget(imageBench);
}

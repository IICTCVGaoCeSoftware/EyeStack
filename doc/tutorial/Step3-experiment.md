# 3. 添加算法代码实验 {#introduce-Experiment}

> 本文写作日期：2022/3/14 - 2022/3/15
> 修订：2022/4/18

在上一节中我们给模板项目的 ALGO 目标添加了一个 Sobel 算法。ALGO 是一个库构建目标，没有 main 函数，算法工作人员需要另外的可执行构建目标来开展算法实验，本节会介绍如何在模板项目的 exp 目标中编写实验代码，并进一步展示 Eyestack 为算法开发人员提供的服务与功能。

现在，我们把注意力放到 Qt Creator 的 EXP 构建目标上：

![](focus_on_exp.png)

打开 main.cpp，里面已经有了一些示例代码：

```cpp
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
```

代码已经有很详尽的注释了，在此我罗列一下涉及到的 Eyestack 功能：

1. `Eyestack::Framework::Application` 类

   它是 Eyestack 的单例模式应用类，它是 Eyestack 框架对上层端到端地提供服务的统一接口。

   它的构造函数完成整个框架的初始化，因此，app 对象越早实例化越好。

   > 你可以把这句话理解为，在 main 函数第一行写这样一句声明是固定的写法。

2. `Eyestack::Base::util` 命名空间

   这个命名空间下放置了许多方便编程的工具函数，比如 OpenCV 与 Qt 图像格式之间的转换等等，此处使用的 exec_widget 可以在对话框中执行一个 QWidget 直到关闭。

3. `qDebug` 宏

   qDebug 宏实际上来自 Qt 框架，但 Eyestack 对其进行了些许修改（主要是输出格式和重定向）用作**Eyestack 日志系统**的一部分。

   具备类似日志功能的还有 `qInfo`、`qWarning`、`qCritical`、`qFatal`，它们代表了不同的日志级别，最严重的级别 `qFatal` 会导致程序直接退出。

我们要编写的算法实验程序与上面基本等同：首先，从文件系统读入一张输入图片，然后让用户配置算法参数，最后使用 Sobel 类处理图片，并在屏幕上显示处理结果。

我们使用 OpenCV 的 imread 来获取图片输入，然后使用 Paramgr 组件配置参数，最后，我们用 Eyestack 提供的 ImageBench 类显示结果，代码如下：

```cpp
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
```

在此对代码进行几点说明：

1. main 函数的前半部分和原先的示例代码基本一致，不再重复。
2. 回忆一下之前 Sobel 类的 \_borderType 参数我们并没有使用参数类，而是设计为属性方法，让外部调用方法间接修改。两种设计在此处的差别就显现了出来，由于按前一种方式设置的参数完全没有出现在 EXP 的代码中，也就意味着 ALGO 代码的变化完全不会引起这里代码的修改，而后者则需要应用程序员为算法的变化承担连带责任，这会带来额外的工作量，并且每次修改都将增加系统的不稳定性。
3. 我们使用 Eyestack 提供的 ImageBench 组件显示图片，这个组件除了显示图片外，也提供了缩放、平移、取色等交互功能，它位于 Eyestack::Design 命名空间下，并且需要包含头文件 `<Eyestack/Design/ImageBench.hpp>`。
4. `PROJECT_SOURCE_DIR` 是一个宏，它被替换为一个路径字符串，这个字符串为当前项目的路径，这是项目模板提供的功能。

我们运行一下看看效果：

1. 首先，在 Qt Creator 中启动程序，屏幕中央出现 Eyestack 的标志，并且下方开始打印日志信息：

![](exp_start_0.png)

2. 然后，弹出了参数配置窗口：

![](exp_start_1.png)

我修改为如下取值：

![](exp_start_2.png)

3. 关闭参数配置窗口，弹出处理结果：

![](exp_start_3.png)

4. 关闭处理结果窗口，程序退出，如果你有耐心，你可以观察一下运行期间打印的所有日志，理解 Eyestack 框架的执行过程。

## 做一点小改进

我们的实验已经成功了，接下来让我们设想这样一个场景：你的算法在实验室里运行正常，但是在实地生产时却产生了错误的输出，当你想分析你算法的问题时，却发现怎么也无法再重现当时的错误或者生产人员总是不能给你提供想要的信息，你开始懊恼为什么不一开始就让算法把关键环节的运行情况打印下来。

但是，在算法运行中打印日志有三个问题：

1. 把图像等数据打印成图片非常耗时。

   第一，有时图像尺寸很大；第二，文件系统 IO 速度很慢；第三，将图像编码为 png 等格式也许要很多计算，这一切都会导致算法的运行速度被严重拖慢。

2. 算法不是在所有情况下都要打印日志。

   比如你只关心那些可能会引发错误的情况，总之，日志打印语句要能在运行时自由地禁用和启用。

3. 日志输出文件的路径冲突。

   路径冲突是一个概率问题，尽管保证一个算法类自己不会冲突地写入日志文件并不难，但如果所有文件都放在一个文件夹内，那么不同程序模块之间发生冲突的概率就变得很大了。而创建子文件夹、确保文件路径有效通常是一个算法研究人员不愿意关心的问题。

Eyestack 的日志系统已经为你解决了这两个问题，它提供了一个日志助手类 `Eyestack::Base::Logger` 来管理你的日志输出，此外，它也内置对多种日志数据类型的支持。

Logger 的用法非常简单，你只需要在想输出日志的地方实例化一个对象，然后调用其上的方法即可。在这里，我希望在 Sobel 类的处理方法中记录一下算法的输入原始数据和输出结果，我们把焦点放回到 ALGO 目标上，修改 Sobel 类的 process 方法为：

```cpp
cv::Mat
Sobel::process(const cv::Mat& src)
{
  namespace esb = Eyestack::Base;
  esb::Logger logger("Getting_Started/");

  logger.image("src.png") << src;

  cv::Mat dst;
  cv::Sobel(src, dst, _ddepth, _dx, _dy, _ksize, _scale, _delta, _borderType);

  logger.image("dst.png") << dst;

  return dst;
}
```

1. 首先，创建一个 logger 对象，其构造函数的第一个参数传入日志记录前缀，这个前缀将被添加到之后输出的所有日志文件上，而斜杠则表示创建一个文件夹，所以，之后的 src.png 和 dst.png 会输出到 Getting_Started 文件夹下。

   > 具体的绝对路径取决于 Eyestack 日志系统的配置，通常位于程序工作目录的 log 文件夹下（运行中），或 log.old 归档文件夹的下的对应目录。

   > 尽管创建 Logger 对象的开销已经被尽可能地降到最低，但仍然不是完全没有，所以，在循环体中声明 Logger 对象是一个坏主意。

2. 调用 logger 对象上的 image 方法输出图片日志。值得注意的是，这行代码是异步的，也就是说，实际上程序运行到这里只是提交了一个日志请求，即告诉 Eyestack：“给我把这张图片打印出去，我先干别的事了”，因此它可以大大降低日志操作对算法运行效率的影响。

   > 异步操作不会影响算法线程的运行，但图片的编码保存过程终究还是要产生计算，因此，过于频繁的日志操作（比如在循环体中输出一千张图片）会导致日志线程挤占其它线程的 CPU 资源，最终导致系统效率降低。

然后，我们再次运行启动 EXP，配置参数、显示结果、关闭程序，然后转到程序工作目录中的 log.old 文件夹，在归档的日志记录中即可看到输出结果：

![](logout.png)

> 若程序没有退出，日志输出不会被归档，此时直接到工作目录下的 log 文件夹查看输出。

我还没有讲怎么在运行时控制日志的输出与否，如果你不想让日志输出，你当然可以在输出语句外面套一个 if 语句，类似这样：

```cpp
if (_enableLogger)
   logger.image("src.png") << img;
```

这样写起来实在是太麻烦了：如果你有 100 个输出语句，就要写配套 100 个 if 语句。

Eyestack 当然不会让你这么做，它只需要你写一行禁用 logger 的语句即可：

```cpp
esb::Logger logger("Getting_Started/");
logger.disable();
```

更妙的是可以把 Logger 与 Paramgr 结合，这样就可以让用户直接控制是否打印日志。

1. 在 Sobel.hpp 中添加一个 Bool 参数：

   ```cpp
   class Sobel
   {
      // ...

      Double _scale{ "scale", "缩放因子", 1 };
      Double _delta{ "delta", "偏移值", 0 };

      Bool _enableLogger{ "enableLogger", "是否启用日志输出", true };
   };
   ```

2. 然后在构造函数中把它注册到参数表里：

   ```cpp
   Sobel::Sobel()
   {
      _paramTable.register_param(_ddepth);
      _paramTable.register_param(_dx);
      _paramTable.register_param(_dy);
      _paramTable.register_param(_ksize);
      _paramTable.register_param(_scale);
      _paramTable.register_param(_delta);

      _paramTable.register_param(_enableLogger);
   }
   ```

3. 在禁用 logger 前进行一个判断：

   ```cpp
   cv::Mat
   Sobel::process(const cv::Mat& src)
   {
      namespace esb = Eyestack::Base;
      esb::Logger logger("Getting_Started/");

      if (!_enableLogger)
         logger.disable();

      // ...
   }
   ```

现在再启动 EXP，你就能从界面上控制日志输出了：

![](enable_logger.png)

## 计时你的算法

在算法开发的过程中，对算法运行时长进行测试是非常常见的需求，Eyestack 对这种需求进行了特别的支持：`Eyestack::Base::Profiler`，下面的的代码演示如何使用它们：

```cpp
cv::Mat
Sobel::process(const cv::Mat& src)
{
  namespace esb = Eyestack::Base;
  esb::Logger logger("Getting_Started/");

  if (!_enableLogger)
    logger.disable();

  logger.image("src.png") << src;

  cv::Mat dst;

  esb::Profiler prof;
  {
    esb::Profiler::Scoper scoper(prof, "cv::Sobel");

    cv::Sobel(src, dst, _ddepth, _dx, _dy, _ksize, _scale, _delta, _borderType);
  }
  logger.profiler("prof") << prof;

  logger.image("dst.png") << dst;

  return dst;
}
```

首先创建一个 prof 对象，然后在要测试的代码段外套个大括号，最后在里面最开头声明一个 scoper，scoper 会自动测试从创建到作用域销毁之间的耗时。

然后，在作用域外，我们使用 logger 的 profiler 函数将 prof 输出到文件中。只是需要注意的是，使用 logger 输出 prof 是一次性的，在这行代码之后，prof 已有数据将被清空。

---

你已经跑通了算法部分的工作流程！接下来：[开发实际应用](app_develop.md)

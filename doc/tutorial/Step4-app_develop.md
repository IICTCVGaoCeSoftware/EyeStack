# 4. 开发实际应用 {#introduce-App_Develop}

> 本文写作日期：2022/3/15 - 2022/3/28
> 修订：2022/4/18

从这一章节开始，我会介绍应用开发人员如何基于 Eyestack 开发用于最终发布给客户使用的应用程序。

现在，把注意力放到项目中的 APP 目标上：

![](target_app.png)

APP 目标位于模板项目的 app 目录内，里面已经有了三个文件：`CMakeLists.txt`、`main.hpp`、`main.cpp`。

CMakeLists.txt 中描述了 APP 目标的构建规则，之后如果想要添加其它源文件就需要同时修改它，具体写法此处不再赘述。

和之前类似，main.hpp 和 main.cpp 里也包含了基本的示例代码，可以先构建运行一下看看效果：

![](app_example.png)

好了，现在我们先复原最初始的状态，然后一步一步地把这个示例代码写出来，首先，清空 main.hpp 和 main.cpp 为：

```cpp
/**
 * @file main.hpp
 */

#pragma once
```

```cpp
/**
 * @file main.cpp
 */

#include "main.hpp"

int main(int argc, char* argv[])
{}
```

然后，编译、构建、启动，程序不会有任何操作，直接退出。

使用 Eyestack 框架的第一步是实例化 Eyestack 应用对象，为此，我们需要包含 `<Eyestack/Framework.hpp>` 头文件，完成这一步的代码为：

```cpp
/**
 * @file main.hpp
 */

#pragma once

#include <Eyestack/Framework.hpp>

namespace esf = Eyestack::Framework;
```

> 项目规范：
>
> 在这里，我们在头文件并且是全局命名空间中使用了 namespace 语句，这是被允许的，因为对于应用程序构建目标而言，其位于依赖树的叶子位置，不会有其它构建目标使用这些头文件，因此命名空间污染问题可控。

```cpp
/**
 * @file main.cpp
 */

#include "main.hpp"

int
main(int argc, char* argv[])
{
  esf::Application app(argc, argv);

  // TODO

  return app.exec();
}
```

主函数最初的写法对于任何上层应用都是固定的：实例化一个 `Eyestack::Framework::Application` 类，然后调用其 app.exec 方法并返回。不同应用的区别在于中间的初始化部分，例如注册组件、设定工作对象等等。Application 是一个单例模式类，它在全局只能创建一个对象，即上面的 app 对象，这个对象就是 Eyestack 为上层项目提供服务的统一接口。

其实现在程序就已经能编译构建运行了，运行一下看看效果：

![](skeleton_run.png)

虽然能够运行，但是很多按钮点击之后没有反应，因为它缺乏处理逻辑，这也就是我们接下来需要编码的内容。

我们下面要为这个骨架添加一个图像捕获器组件，实现最开始示例代码的效果。图像捕获器位于 Eyestack 的 Gencom 模块下，首先包含头文件，然后在主函数中声明组件：

```cpp
#include "main.hpp"

#include <Eyestack/Gencom.hpp>

namespace esg = Eyestack::Gencom;

int
main(int argc, char* argv[])
{
  esf::Application app(argc, argv);
  esg::ginit(); // Gencom 使用前需要初始化

  esg::VideoCapture vc;

  return app.exec();
}
```

但是，编译器提示我们 vc 对象的构造函数缺少一个参数：

![](video_capture.png)

因此，为了使用这个组件，必须还要有一个工作对象：

```cpp
/**
 * @file main.hpp
 */

#pragma once

#include <Eyestack/Framework.hpp>

namespace esf = Eyestack::Framework;

class Worker : public esf::Worker
{
  // Worker interface
protected:
  virtual void work() override;
};
```

> `Eyestack::Framework::Worker` 类是是系统工作控制的接口，上层应用程序通过实现这个接口来定制自己的工作逻辑。Eyestack 会将工作对象放在一个工作子线程中执行以避免长时间占用主线程拖慢界面交互，同时充分利用多核资源，因此在编写工作对象时需注意线程安全问题。

Worker 类必需要实现一个 work 方法，在 main.cpp 中添加：

```cpp
void
Worker::work()
{}
```

然后，我们在主函数中实例化这个类，并把它注册到 app 中：

```cpp
int
main(int argc, char* argv[])
{
  esf::Application app(argc, argv);
  esg::ginit();

  auto worker = new Worker();
  app.reg_worker(worker);

  // ...
}
```

注意 Worker 必须使用 new 分配在堆上，因为 reg_worker 是移交语义。

现在我们可以用 Worker 实例化视频捕获器了：

```cpp
int
main(int argc, char* argv[])
{
  esf::Application app(argc, argv);
  esg::ginit();

  auto worker = new Worker();
  app.reg_worker(worker);

  esg::VideoCapture vc(*worker);

  return app.exec();
}
```

然后为组件创建界面，通常，组件的内置界面类为组件类下的 SubUi 嵌套类，例如 VideoCapture 类的内置界面类为 `VideoCapture::SubUi`。每个界面类各有各的使用方式，使用时需参考其自身的文档。

```cpp
int
main(int argc, char* argv[])
{
  esf::Application app(argc, argv);
  esg::ginit();

  auto worker = new Worker();
  app.reg_worker(worker);

  esg::VideoCapture vc(*worker);
  esg::VideoCapture::SubUi vcSubUi(vc, "视频捕获器");
  app.reg_sub_ui(vcSubUi);

  return app.exec();
}
```

编译启动，就可以看到视频捕获器了。

用同样的方式，我们在系统中添加一个监视器组件：

```cpp
int
main(int argc, char* argv[])
{
  esf::Application app(argc, argv);
  esg::ginit();

  auto worker = new Worker();
  app.reg_worker(worker);

  esg::VideoCapture vc(*worker);
  esg::VideoCapture::SubUi vcSubUi(vc, "视频捕获器");
  app.reg_sub_ui(vcSubUi);

  esg::Monitor mo(*worker);
  esg::Monitor::SubUi moSubUi(mo, "监视器");
  app.reg_sub_ui(moSubUi);

  return app.exec();
}
```

## 添加程序的工作逻辑

现在，我们把上面两个章节中开发的算法加入系统。我们要达到的目标是：从视频捕获器中读取一帧图像，使用算法处理图像，然后将结果显示在监视器上。

算法应该在工作线程中被执行，因此我们修改 Worker 类，在类中实例化算法对象，并且修改 Worker 的工作函数，工作函数运行时需要从视频捕获器 vc 读取数据、向监视器 mo 写入数据，因此，我们可以将 vc 和 mo 的声明移入 Worker 类中，下面是修改后的 main.hpp 与 main.cpp 文件：

```cpp
/**
 * @file main.hpp
 */

#pragma once

#include "algo/Sobel.hpp"
#include <Eyestack/Framework.hpp>
#include <Eyestack/Gencom.hpp>

using namespace Getting_Started;

namespace esf = Eyestack::Framework;
namespace esg = Eyestack::Gencom;

class Worker : public esf::Worker
{
public:
  esg::VideoCapture _vc{ *this };
  esg::Monitor _mo{ *this };
  Sobel _sobel;

  // Worker interface
protected:
  virtual void work() override;
};
```

```cpp
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

  return app.exec();
}

void
Worker::work()
{
  cv::Mat mat;
  if (_vc.read(&mat))
    _mo.display(_sobel.process(mat));
}
```

编译运行，程序的工作逻辑就填充好了：

![](worker_ready.png)

## 给算法添加配置界面

注意到在算法实验环节中，调用 Sobel 对象进行图像处理前我们还对 Sobel 对象上的参数进行了配置，而上面的代码并没有 Sobel 的参数配置界面，在这一小节中我们将包装 Sobel 对象，给它创建界面，并添加界面到应用程序中。

在开始写代码之前，让我们先思考这样一个问题：

**如果用户在工作线程运行的过程中修改了 Sobel 对象的参数怎么办？**

在这种情况下，主线程和工作线程同时操作 Sobel 对象会导致竞争问题，由于我们的 Sobel 对象上并没有特殊的线程安全机制，这极有可能引发未定义行为，虽然在我们的例子中大概率不会导致程序崩溃，但极有可能导致计算结果错误。

这个问题有三种解决方案：

1. 修改 Sobel 类的代码，使其线程安全
2. 通过阻止用户在运行时修改参数来避免问题出现
3. 使 Sobel 对象为工作线程独占，主线程通过请求工作线程间接操作对象

在当前的语境中，第一种方案不太可取，因为算法类的代码通常有算法开发人员编写，应用开发人员没有修改权，而第二和第三两种方案都有一定的适用场景。

### 阻止用户在运行时修改参数

注意到 Sobel 对象上的参数表（get_param_table）在应用程序中是被直接注册到一个 Paramgr 然后显示在界面上的，所以我们无法在用户修改参数的时候弹出“禁止修改”的提示框，而只能通过禁用界面的方式阻止用户操作。

首先，我们创建一个 SobelWrapper 类取代 Worker 类中原来的 Sobel 对象，监视工作线程的状态，向界面发送禁用和使能信号：

```cpp
/**
 * @file main.hpp
 */

// ...

class SobelWrapper : public QObject
{
  Q_OBJECT

  using _T = SobelWrapper;
  using _S = QObject;

public:
  Sobel _sobel;

signals:
  void s_workerStarted();
  void s_workerStopped();
  void s_workerPaused();
  void s_workerTriggerd();

public:
  SobelWrapper(esf::Worker& worker)
  {
    connect(&worker,
            &esf::Worker::s_started,
            this,
            &_T::s_workerStarted,
            Qt::DirectConnection);
    connect(&worker,
            &esf::Worker::s_stopped,
            this,
            &_T::s_workerStopped,
            Qt::DirectConnection);
    connect(&worker,
            &esf::Worker::s_paused,
            this,
            &_T::s_workerPaused,
            Qt::DirectConnection);
    connect(&worker,
            &esf::Worker::s_triggerd,
            this,
            &_T::s_workerTriggerd,
            Qt::DirectConnection);
  }
};
```

> 在项目规范中，SobelWrapper 的构造函数过长，不适合写成内联函数，此处仅出于方便考虑。

注意信号连接的方式我们显式地设置为 `Qt::DirectConnection`，因为我们需要让工作线程等待界面被禁用后才能继续往下运行。

然后我们可以编写 Sobel 参数表的界面类了，我们使用 Eyestack 框架提供的默认界面拓展类 `Eyestack::Framework::MainWindow::SubUi::MdiAction` 来实现我们的界面：

```cpp
/**
 * @file main.hpp
 */

// ...

class SobelUi : public esf::MainWindow::SubUi::MdiAction
{
  Q_OBJECT

  using _T = SobelUi;
  using _S = esf::MainWindow::SubUi::MdiAction;

public:
  SobelUi(SobelWrapper& wrapper, const QString& name)
    : _S(name)
  {
    _paramgr.set_table(wrapper._sobel.get_param_table());
    setWidget(&_paramgr);

    connect(&wrapper,
            &SobelWrapper::s_workerStarted,
            this,
            &_T::disableParamgr,
            Qt::BlockingQueuedConnection);
    connect(&wrapper,
            &SobelWrapper::s_workerStopped,
            this,
            &_T::enableParamgr,
            Qt::QueuedConnection);
    connect(&wrapper,
            &SobelWrapper::s_workerPaused,
            this,
            &_T::enableParamgr,
            Qt::QueuedConnection);
    connect(&wrapper,
            &SobelWrapper::s_workerTriggerd,
            this,
            &_T::disableParamgr,
            Qt::BlockingQueuedConnection);
  }

private:
  Eyestack::Design::Paramgr _paramgr;

private slots:
  void enableParamgr() { _paramgr.setEnabled(true); }
  void disableParamgr() { _paramgr.setEnabled(false); }
};
```

> 事实上 SobelUi 可以声明为 SobelWrapper 中的一个嵌套类“SubUi”，正如 Eyestack 框架中的内置组件那样。

记住 SobelWrapper 运行在工作线程中，其上的信号将在工作线程中被发射，所以在启动前我们使用 BlockingQueuedConnection 的连接方式，让工作线程等待界面被禁用之后再开始工作，而对于停止要求则没有那么严格，我们改用 QueuedConnection 连接。

最后，在 Worker 中声明 SobelWrapper 类型的成员变量，然后在 main 函数中创建并注册界面：

```cpp
/**
 * @file main.hpp
 */

// ...

class Worker : public esf::Worker
{
public:
  esg::VideoCapture _vc{ *this };
  esg::Monitor _mo{ *this };
  SobelWrapper _sw{ *this };

  // Worker interface
protected:
  virtual void work() override;
};
```

```cpp
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

  // ...

  SobelUi swSubUi(worker->_sw, "Sobel");
  app.reg_sub_ui(swSubUi);

  return app.exec();
}

void
Worker::work()
{
  cv::Mat mat;
  if (_vc.read(&mat))
    _mo.display(_sw._sobel.process(mat));  // 同时需要修改
}
```

编译运行，就可以看到界面了：

![](sobel_subui.png)

### 使工作线程独占 Sobel 对象

Sobel 对象上还有一个 `void set_border_type(int val);` 方法，我们还没有它的设置界面，对这个方法的包装可以使用第三种方案。

首先，修改 SobelWrapper 的构造函数，将其父对象设置为 Worker，然后给 SobelWrapper 添加一个槽用于响应主线程修改属性的请求：

```cpp
class SobelWrapper : public QObject
{

  // ...

public:
  SobelWrapper(esf::Worker& worker)
    : _S(&worker)
  {
    // ...
  }

public slots:
  void set_border_type(int val) { _sobel.set_border_type(val); }
};
```

然后在 SobelUi 类中添加一个组合框，在每次组合框取值变化时，发射信号激发 SobelWrapper 上的槽：

```cpp
class SobelUi : public esf::MainWindow::SubUi::MdiAction
{
  // ...

public:
  SobelUi(SobelWrapper& wrapper, const QString& name)
    : _S(name)
  {
    _container.setLayout(&_mainLayout);
    setWidget(&_container);

    _mainLayout.addWidget(&_comboBox);
    _comboBox.addItem("BORDER_DEFAULT", cv::BORDER_DEFAULT);
    _comboBox.addItem("BORDER_ISOLATED", cv::BORDER_ISOLATED);
    _comboBox.addItem("BORDER_REFLECT", cv::BORDER_REFLECT);
    connect(&_comboBox, &QComboBox::currentIndexChanged, this, [this](int i) {
      emit change_sobel_borderType(_comboBox.itemData(i).toInt());
    });
    connect(this,
            &_T::change_sobel_borderType,
            &wrapper,
            &SobelWrapper::set_border_type,
            Qt::QueuedConnection);

    _mainLayout.addWidget(&_paramgr);
    // ...
  }

private:
  QWidget _container;
  QVBoxLayout _mainLayout;
  Eyestack::Design::Paramgr _paramgr;
  QComboBox _comboBox;

signals:
  void change_sobel_borderType(int type);

  // ...
};
```

采用这种方案的话，无论工作线程处于什么状态，用户都可以修改 Sobel 的属性，只是什么时候 Sobel 属性真正被修改是异步的，用户可能看到运行结果和参数不符的情况，这个问题有多种解决方式，不在此展开论述。

到此为止，我们的应用开发就完成了。

---

接下来：[进入发布流程](Release.md)

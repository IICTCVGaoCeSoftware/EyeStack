# Workctrl - 工作控制 {#guide-workctrl}

设计工作控制服务主要目的是通过一个工作线程，允许上层应用通过该服务将 CPU 密集型任务从主线程（UI 线程）中提取出来，从而降低系统的操作时延、提高应用程序的界面友好度。另一个目的是通过制定一个规范的工作控制模型以允许上层应用组件化、可复用。

> 关于应用程序的运行时结构，详见[应用程序模型文档](app_model.md)。

设计上，上层应用只需提供一个继承 Worker 接口的工作对象，描述如何**在工作线程中**响应工作控制服务的控制信号，而无需关心控制信号在什么时候发生以及工作线程如何管理。

在创建好工作对象后，使用 Workctrl 的 set_worker 设置工作对象：

```cpp
class MyWorker: public Workctrl::Worker
{
  // ...
} myWorker;
Application::workctrl().set_worker(myWorker);
```

然后，框架将根据用户在界面上的操作控制 myWorker 的工作状态。

## 默认控制模型

![](workctrl_model.png)

Worker 接口的默认工作控制模型如上图所示，模型由 3 个状态、4 个方法组成。

1. 停止状态与 stop 方法

   停止状态是系统启动时的默认状态，用于表征检测系统没有在运行，在其它状态下通过 stop 方法进入停止状态。

2. 运行状态与 start 方法

   运行状态用于表征检测系统在无人干预的情况下自动运行，在工作对象的默认实现中，运行状态会在工作线程中启动一个计时器，从而让工作线程自发地周期工作。start 是进入运行状态的对应方法。

3. 暂停状态与 pause 方法

   暂停状态用于表征有人干预的系统运行状态。在这种状态下，系统会在每次收到触发信号时执行一次工作。暂停状态设计用于方便系统的调试和分析，pause 方法是进入暂停状态的对应方法。

4. trigger 方法

   trigger 方法用于发送触发信号，在工作对象的默认实现中，只有在暂停状态下才会响应触发信号。

工作对象有 5 个信号来对外告知自己的状态：s_started、s_stopped、s_paused、s_triggerd、s_failed，前 4 个信号在工作对象接受对应的控制信号时发射，如果在工作对象运行过程中抛出异常，就会发射 s_failed 信号，默认实现中工作对象在抛出异常后会转入停止状态。

## 控制模型的定制

前文图中的状态转换都不是必定发生的，Workctrl 服务发出的 start 等控制信号应该理解为一种请求，而非命令，状态根本上由运行在工作线程中的工作对象决定。例如，上层应用可以定制工作对象，使其在 start 信号的处理函数中什么也不做，从而实现忽略 start 信号的效果。

控制模型允许定制的原理是，Worker 接口的信号响应函数都是虚函数，因此，上层可以通过重载这些响应函数来实现修改信号的响应逻辑。

通过继承 Worker 接口定制控制模型时，在方法重载中调用相应的状态转换函数来向外界通知状态的转变。例如，可以在工作对象的工作方法中显式调用停止方法，从而在工作出现异常（例如检出缺陷）时停止工作。

由于上层可以重载信号响应函数，控制模型的停止、运行、暂停状态的实际含义也可以发生改变。应将默认控制模型的实现视为一种参考，框架没有限制上层应用程序对系统运行状态的定义。
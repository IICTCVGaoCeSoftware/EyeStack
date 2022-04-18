# 框架提供的服务 {#guide-services}

> 本文写作日期：2022/3/29 - 2022/4/1

Application 类是应用开发框架为上层应用开发提供的服务的统一、集中接口。框架提供的所有服务都在该类上得到体现，每个服务对应 Application 类的一个静态方法：

```cpp
Application::logsys();      // 日志系统
Application::configurer();  // 配置管理
Application::workctrl();    // 工作控制
Application::main_window(); // 主界面
Application::notifier();    // 消息通知
Application::bgtasker();    // 后台任务
```

通过进一步调用这些服务模块对象上的方法来使用各种功能，例如使用 notifier 发送一则通知：

```cpp
Application::notifier().notify_text("hello, world!");
```

## 在使用服务时确保应用已经创建

虽然这些方法是静态的，但他们都必须在 Application 实例化之后才能调用。Application 是一个单例模式类，按照设计意图，它应该在 main 函数中被尽可能早地创建，并且整个进程全局只能创建一个实例对象。

如果不加小心，有可能写出在构造 Application 对象之前就调用服务方法的代码。例如，你创建了一个自定义类：

```cpp
class MyClass
{
public:
  MyClass() {
    Application::notifier().notify_text("对象已创建");
  }
};
```

然后用这个类实例化了一个全局或静态对象：

```cpp
static MyCalss gMyclass;
```

那么程序就会在进入主函数前就调用 Application 的 notifer，从而崩溃退出。

## Application 类上的便捷方法

Application 类为常用服务提供了一些便捷方法，例如：

```cpp
Application::configurer().register_config(myConfig);
```

等同于

```cpp
Application.reg_config(myConfig);
```

# 目录

- \subpage guide-logsys
- \subpage guide-configurer
- \subpage guide-workctrl
- \subpage guide-main_window
- \subpage guide-notifier
- \subpage guide-bgtasker

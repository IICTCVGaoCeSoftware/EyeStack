# Notifier - 消息通知 {#guide-notifier}

相比于日志系统向开发人员输出信息，消息通知向最终的应用程序用户输出信息。当系统出现需要外力介入的操作时，可以通过消息通知服务向操作员报告系统状态。

框架提供了 Notifier 作为系统发送通知消息的统一方式。**Notifer 是线程安全的**，在任何线程中，都可以通过其上的方法发送响应类型的消息：

```cpp
Application::notifier().notify_text("This is a notification.");
```

> 关于 Notifier 上的方法如何使用，详见 Notifier 类的文档。

这将使界面上弹出消息框并显示一则通知：

![](notification.png)

Notifier 在发送每种类型的消息在时都会同时发射相关的信号，外界可以通过连接这些信号来监听相关的通知。

## 自定义类型通知

Notifier 仅内置了对基本文本类型消息的通知，如果想要在通知框中显示图片等类型的消息，可以通过 Notifier 的通用类型消息通知方法 `notifiy` 实现。

notify 方法接受三个参数，第一个是返回 QWidget* 的函数对象，第二个是通知类型，第三个是通知标题，函数对象将在主线程中被执行以创建显示界面所需的 QWidget 对象。下面是一个使用示例：

```cpp
QImage img;

// ...

Application::notifier().notify([img]() {
  auto label = new QLabel();
  label->setPixmap(QPixmap::fromImage(img));
  return label;
});
```

编写 lambda 函数时，外部变量一般采用传值捕获的方式，因为消息通知的过程是跨线程异步的，传递引用往往会导致指针悬挂。

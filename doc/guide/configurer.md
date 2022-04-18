# Configurer - 配置管理 {#guide-configurer}

机器视觉缺陷检测系统中往往会有大量的参数，为了避免每次启动系统都进行繁琐的配置操作，将系统中本次的运行参数等保存为一个文件，然后在下次运行时加载是一种常见做法。Eyestack 提供了一个 Configurer 类来辅助上层应用管理配置，使用该服务，在开发上层应用程序时就无需操心配置文件如何存储，而只需专注于如何将配置序列化的部分。

要使用 Configurer 类保存你的数据，首先要继承 Configurer 类中的 `Config` 接口，接口只有三个方法，分别用于重置、序列化、反序列化。在实现接口时，有以下几个注意点：

1. **重置方法绝不能抛出异常，必须保证成功**，因为其会在反序列化失败时用作回退，以免系统进入未定义状态。

2. 序列化与反序列化方法允许抛出异常，并且框架中已经为此准备了 `DumpingError` 和 `LoadingError` 两个运行时异常类方便使用。

3. **框架将在主线程中执行接口对象上的方法**，注意配置保存时的线程安全性。

创建好配置对象后，使用 Configurer 上的 register_config 来注册它：

```cpp
MyConfig myConfig;
Application::configurer().register_config(myCnofig);
```

注意 register_config 对 myConfig 是借用语义，在编写上层程序时必须保证 configurer 调用配置对象时最初注册的指针仍然有效。

此外，Configurer 类还有三个用于重置、保存、加载所有配置对象的方法，这些方法主要被框架调用，但也可被上层程序调用以主动获取当前配置。由于 Configurer 不是线程安全的，所以同样要注意，这些方法只应该在主线程中被调用。

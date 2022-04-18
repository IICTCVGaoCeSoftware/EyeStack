# Logsys - 日志系统 {#guide-logsys}

对于任何系统而言，记录日志都是一件十分必要的工作，其目的并不在于服务用户，而在于服务开发人员，当程序发生错误崩溃退出时，如果记录了详细的日志，那么将会给错误的排查指明方向。设计上，在 Eyestack 上层应用程序[正常的运行流程](app_model.md)中，日志系统在整个应用程序中最先启动，最晚退出，以确保不会遗漏所有日志打印消息。

## 日志与归档

日志系统启动时会在当前工作目录下创建一个 `log` 目录，并将所有日志文件放在其中，在最后退出时，日志系统会将 `log` 目录移动到 `log.old` 目录（如果没有则创建）下实现归档，使得程序再次启动时不会覆盖此前的日志文件。以上所有目录位置都是默认情况下的配置，可以在上层项目中修改。

可以通过 Logsys 的 `log_path` 方法获得当前日志目录的路径，通过 `arc_path` 方法获得当前归档目录的路径。在运行中修改日志目录很容易会带来问题，因此这两个目录在程序启动时即确定，且在整个程序的运行过程中不允许变动。

## 存储监控

日志记录的另一个问题是当系统长时间运行时，积累的日志会占用很大的磁盘空间。为此，Logsys 会在系统运行过程中定时检查日志目录和归档目录的大小，当检查到存储占用超过 2GB（默认值）时，框架会自动通知用户清理日志目录。

## 文本日志

通常，日志记录都是将一些嵌入在程序中的字符串写入到一个文本文件中，为了方便分析，每条记录还会带上时间戳、严重级别、分类标签、代码位置等信息。Eyestack 基于 QtDebug 模块来满足这种需求，QtDebug 提供 `qDebug`、`qInfo`、`qWarning`、`qCritical`、`qFatal` 五个日志记录宏，分别对应五个日志级别，其中最严重的 `qFatal` 将在写入日志后导致程序崩溃退出。

这些宏的使用请参考 Qt 中的相关文档，下面是 Logsys 默认的日志格式：

```
[日期 时间 严重性 标签 类别] "函数名" "文件" 行号 <换行>
日志主体内容
```

其中函数名、文件、行号在发布版本中会被省略，下面是一个示例：

```
[2022-Mar-31 15:47:01.058911 Info Workctrl 2] "void __cdecl Eyestack::Framework::Workctrl::run(void)" "D:\WAS\IICT\projects\Eyestack\module\Framework\Workctrl.cpp" 183
工作线程 Eyestack::Framework::Workctrl(0x1dccb90a3b0) 已启动。
```

## 异步日志记录

对于机器视觉应用而言，除了文本字符串，往往还要记录图片、视频、算法参数、性能分析等等信息。与此同时，机器视觉应用往往都是要求高性能的应用程序，对于图片数据类型，一方面其体积常常很大，另一方面将其编码为常见的图像存储格式也需要很多计算，因此，在算法处理过程中进行日志记录会延长算法执行周期，影响系统总体效率。

因此，Logsys 提供了异步日志记录的服务，其内部通过一个线程池来从工作线程中接管日志记录操作，从而隐藏日志记录的延迟。这个服务通过 Logger 类提供，如果想使用这个服务，在任何地方、任何线程中创建一个 Logger 对象，然后调用 Logger 对象上的方法，例如：

```cpp
cv::Mat img(1000, 1000, CV_8UC3);
// 在 img 上画画
Logger logger;
logger.image("a.jpg") << img;
```

上面的代码将 OpenCV Mat 类型的图片 img 写入到了日志目录下的 "a.jpg" 文件中（自动编码为 jpg 格式），日志写入实际在线程池线程中进行。

直接将文件写入到日志根目录下容易产生文件名冲突，在指定路径时可以加上路径前缀：

```cpp
logger.image("b/a.jpg");
```

这会在日志目录下创建 b 文件夹（如果不存在），然后将图片存储到 a.jpg 文件中。

也可以在创建 Logger 时指定前缀，这个前缀将加载所有后续的日志记录操作上：

```cpp
Logger logger("a/b/c");
logger.image("a.png") << mat; // 存储到日志目录下的 a/b/c/a.png 文件中
```

更多详细信息参考 Logsys 类的文档。

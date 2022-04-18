# 2. 开发一个算法 {#introduce-Add_Sobel}

> 本文写作日期：2022/3/14 - 2022/3/15
> 修订：2022/4/18

在本章节和下一章节，我们为项目模板添加一个算法并进行实验，在此过程中同时演示 Eyestack 为算法开发人员提供了哪些服务。

我们要做的是将图像梯度的 Sobel 算子封装为算法，其原理不再赘述。之所以选择这个算子一是因为简单，二是因为 OpenCV 中有现成的函数可以调用，我们先来看一下 OpenCV 里面的函数签名是什么样：

```cpp
void
cv::Sobel(InputArray src,
          OutputArray dst,
          int ddepth,
          int dx,
          int dy,
          int ksize = 3,
          double scale = 1,
          double delta = 0,
          int borderType = BORDER_DEFAULT);
```

其中算法输入除了图像 src 之外，还有若干参数（ddepth、dx、dy……），我设计的封装方法是这样：把除了 src 之外的其它参数视为先验参数，由用户提前设定，而在算法运行过程中不变。

首先，分别在 algo/inc/algo 文件夹和 algo 文件夹下创建头文件 Sobel.hpp 和源文件 Sobel.cpp：

![](add_file.png)

添加了头文件后我们需要把他们添加到 CMake 清单文件中：

![](add_source.png)

保存清单文件，不出意外 Qt Creator 会自动加载项目，随后你会在 ALGO 目标下看到添加的两个文件：

![](new_source.png)

> **之后添加的源代码文件也是同样的过程，不再赘述。**

然后我们就可以开始写代码了，先编写 Sobel.hpp，给我们的算法类搭个架子：

```cpp
/**
 * @file Sobel.hpp
 */

#pragma once

namespace Getting_Started {

/**
 * @brief Sobel 图像梯度算子，包装自 OpenCV 同名函数。
 */
class Sobel
{
public:
  Sobel();

public:
  /**
   * @brief 处理图片
   * @param img 输入图像
   * @return 输出图像
   */
  cv::Mat process(const cv::Mat& src);

private:
  int _ddepth, _dx, _dy, _ksize, _borderType;
  double _scale, _delta;
};

}
```

几点解释：

1. 使用命名控件包含所有代码以避免名称冲突，方便后面的代码维护与对接

2. cv::Mat 定义在 OpenCV 的头文件中，但我们并没有编写 `#include <opencv>`，这就是因
   为项目模板的预处理头文件（\_PCH）已经包含它们了。

   > 预处理头文件机制的使用也是 Eyestack 提供的服务之一。

3. 成员变量前加下划线是推荐的代码规范，实际项目可能有不同的代码规范。

4. 将成员变量设为 private 这体现了面向对象的封装特征。

   > 在设计类方法的时候，要思考应用程序员如何使用你的算法，时刻提醒自己设计模式的七大原则！

然后，我们在 Sobel.cpp 中填充代码：

```cpp
/**
 * @file Sobel.cpp
 */

#include "algo/Sobel.hpp"

namespace Getting_Started {

Sobel::Sobel() {}

cv::Mat
Sobel::process(const cv::Mat& src)
{
  cv::Mat dst;
  cv::Sobel(src, dst, _ddepth, _dx, _dy, _ksize, _scale, _delta, _borderType);
  return dst;
}

}
```

然后这个类就可以通过编译了，但它还没有完成，因为目前还没有给外界提供修改 \_ddepth 这些参数的方法。

我们可以用一般 C++ 代码的写法，为成员变量编写 getter、setter 方法，在 process 后添加：

```cpp
  /**
   * @brief 获取 borderType 参数
   */
  int get_border_type() { return _borderType; }

  /**
   * @brief 设置 borderType 参数
   */
  void set_border_type(int val) { _borderType = val; }
```

这种设计就需要应用程序员调用这些函数，编写把参数值从界面上转手到类调用的代码，这些代码通常写起来不算多，但绝对写起来很无聊。此外，因为这样做把参数作为了接口的一部分，当算法需要的参数发生变动时，就需要应用程序员修改它的代码。

Eyestack 已经考虑到了这种应用场景，为此，它提供了一个参数管理器来解决这种需求。首先，在 Sobel.hpp 中引入参数管理器头文件：

```cpp
#include <Eyestack/Design/Paramgr.hpp>
```

命名空间前缀太长了，为了使用简单，我们在 Getting_Started 命名空间中给它们起个别名：

```cpp
#include <Eyestack/Design/Paramgr.hpp>

namespace Getting_Started {

namespace esd = Eyestack::Design;

// ... 其它代码
}
```

> 项目规范：
>
> **为了避免污染全局命名空间，千万不要把这行别名写到全局命名空间，更不要在全局命名空间内使用 using namespace 语句！**

然后，我们修改 Sobel 类的定义，添加一个参数表成员变量，并且加上一个让外界只读的方法：

```cpp
class Sobel {
  // ...

  /**
   * @brief 获取算法的参数表
   */
  esd::Paramgr::Table& get_param_table() { return _paramTable; }

private:
  esd::Paramgr::Table _paramTable;

  int _ddepth, _dx, _dy, _ksize, _borderType;
  double _scale, _delta;
};
```

有了参数表之后，我们需要告诉参数表我们有哪些参数，为此算法开发人员还需要提供它们的名字、描述等信息，这些信息 C++ 的原始类型肯定是包含不了的，为此，Eyestack 在 `Eyestack::Design::paramgr` 命名空间下内置了许多参数类型，例如 `Eyestack::Design::paramgr::Int` 是与 C++ int 类型完全兼容的高级参数类。

为了方便使用，我们在之前的命名空间别名语句后加上一行：

```cpp
namespace esd = Eyestack::Design;
using namespace esd::paramgr;
```

然后我们修改 \_ddepth 等几个参数的类型为 Eyestack 提供的参数类型：

```cpp
class Sobel
{
  // ...

private:
  esd::Paramgr::Table _paramTable;

  int _borderType;
  Int _ddepth, _dx, _dy, _ksize;
  Double _scale, _delta;
};
```

这些参数类没有无参构造函数，我推荐使用 C++ 11 之后的类内初始化语法来初始化：

```cpp
class Sobel
{
  // ...

private:
  esd::Paramgr::Table _paramTable;

  int _borderType;

  Int _ddepth{ "ddepth", "输出图像深度", CV_8U };
  Int _dx{ "dx", "X 方向导数阶", 1 };
  Int _dy{ "dy", "Y 方向倒数阶", 1 };
  Int _ksize{ "ksize", "算子核大小", 3 };

  Double _scale{ "scale", "缩放因子", 1 };
  Double _delta{ "delta", "偏移值", 0 };
};
```

然后，我们还要把这些参数注册到参数表上，在 Sobel.cpp 中编写 Sobel 类的构造函数：

```cpp
/**
 * @file Sobel.cpp
 */

#include "algo/Sobel.hpp"

namespace Getting_Started {

Sobel::Sobel()
{
  _paramTable.register_param(_ddepth);
  _paramTable.register_param(_dx);
  _paramTable.register_param(_dy);
  _paramTable.register_param(_ksize);
  _paramTable.register_param(_scale);
  _paramTable.register_param(_delta);
}

// ...
}
```

就这样，算法类完成了，不需要修改 Sobel.cpp 其它涉及到变量使用的代码，因为在绝大多数情况下，这些高级参数类都与原始类型兼容。

---

接下来：[添加算法代码实验](experiment.md)

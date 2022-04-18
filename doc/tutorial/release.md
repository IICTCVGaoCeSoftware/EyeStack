# 5. 测试、文档、打包、部署 {#introduce-Release}

> 本文写作日期：2022/3/28 - 2022/3/29

万事俱备，只欠东风，我们以及完成了所有的开发工作，接下来就是把应用发布出去。本章教程介绍 Eyestack 框架对于编写测试、生成文档、构建打包和应用部署上的解决方案。

## 编写测试

Eyestack 生成的模板项目中已经内置了对 Boost.Test 的支持，位于 test 目录下，其中已经添加了一个示例测试：

```cpp
#define BOOST_TEST_MODULE Test_1
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_1)
{
  BOOST_TEST(true);
  BOOST_TEST_MESSAGE("hello, test!");
}
```

Boost.Test 的使用请参阅官方文档：https://www.boost.org/doc/libs/1_76_0/libs/test/doc/html/index.html。

在编写测试时，只需要记住：

1. Eyestack 框架的大部分功能都需要创建 Application 后才能运行。
2. 一些模块（例如 Gencom）在使用前需要调用 ginit 初始化。

因此，建议使用 Boost.Test 的全局测试夹具进行初始化工作：

```cpp
#include <Eyestack/Framework.hpp>

struct GlobalFixture
{
  Eyestack::Framework::Application app{ 0, nullptr };

  GlobalFixture() {
    Eyestack::Gencom::ginit();
  }
  ~GlobalFixture() {}

  void setup() {}
  void teardown() {}
};

BOOST_TEST_GLOBAL_FIXTURE(GlobalFixture);
```

## 生成文档

Eyestack 生成的模板项目采用 Doxygen 作为文档生成工具，位于 doc 目录下，目录中已经添加了一个名为 DOC 的构建目标：

```cmake
set(DOXYGEN_IMAGE_PATH .)         # 设置图片搜索目录
set(DOXYGEN_USE_MATHJAX YES)      # 启用数学公式
set(DOXYGEN_CREATE_SUBDIRS YES)



# 添加文档构建目标
doxygen_add_docs(DOC
  # 主页
  index.md

  # 提取代码中的注释
  ../algo ../app
)
```

Doxygen 可以自动从源代码文件中提取注释，只需在注释代码时使用 Doxygen 语法“/** ... */”,正如此前教程里写的那样：

```cpp
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
   * @param src 输入图像
   * @return 输出图像
   */
  cv::Mat process(const cv::Mat& src);

  // ...
};
```

注释编写支持 Markdown 语法。

除了提取代码中的注释外，Doxygen 还可以插入 Markdown 文档生成独立页面，就像上图的 index.md 那样，详细使用请查看 Doxygen 文档。

## 构建打包

Eyestack 提供了基于 CPack 的打包发布方案，但仍需要上层项目具体编写部署规则。

打开模板项目根目录的 CMakeLists.txt，你会在最后看到：

```cmake
#
# 部署配置
#
install_eyestack_runtime(APP MODULE Gencom)

get_target_property(_type ALGO TYPE)
if (_type STREQUAL "SHARED_LIBRARY")
  install(TARGETS ALGO RUNTIME)
endif()
```

install_eyestack_runtime 是 Eyestack 为编写发布应用的打包规则而提供的便捷函数，其第一个参数为待发布的应用主程序，在 MODULE 后跟上所使用的 Eyestack 模块，Eyestack 会自动添加所有关联依赖的运行时。除此之外，上层项目的其它运行时文档需要另外编写打包规则，正如上面的 ALGO 目标一样。

在编写好打包规则后，切换到 Release 构建版本，构建应用。然后在运行环境中打开命令行：

![](open_cmd_with.png)

进入 Release 构建目录，执行 CPack 命令：

![](cpack_g_zip.png)

图中红框即用于发布的打包结果。

除压缩包外，CPack 还支持 NSIS、WIX 等可执行文件的打包方式，详见 CPack 官方文档。

## 应用部署

如果打包规则编写正确，那么上一步中打出的压缩包应该是一个自包含的目录，在部署环境中解压压缩包，运行 bin 目录下的对应可执行文件，程序即可启动。

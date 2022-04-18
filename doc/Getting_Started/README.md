# Traditional 开发模板

本项目模板用于传统机器视觉应用的合作式开发。

## 项目结构

```
|   .gitignore
|   CMakeLists.txt      # CMake 清单文件
|   deps.cmake          # 公共 CMake 依赖
|   env.cmake.example   # CMake 环境解耦文件样例
|   README.md
|   version.hpp.in      # 生成版本号头文件输入
|   _pch.cpp            # 预编译源文件（内容为空）
|   _pch.hpp            # 预编译头文件
|
+---algo                # 算法源码目录
|   |   Adder.cpp
|   |   algo.cpp
|   |   CMakeLists.txt
|   |
|   \---inc             # 算法公共头文件目录
|       |
|      ...
|
+---app                 # 应用源码目录
|       CMakeLists.txt
|       main.cpp
|       main.hpp
|
+---doc                 # 文档生成源码目录
|       CMakeLists.txt
|       index.md
|
+---exp                 # 算法实验源码目录
|       CMakeLists.txt
|       main.cpp
|       main.hpp
|
\---test                # 应用测试源码目录
        CMakeLists.txt
        test_1.cpp
```

## 使用方式

项目被设计用于解耦算法和应用代码，方便算法开发人员和应用开发人员的对接合作。

1. 对于算法开发人员：

   algo 目录是算法开发人员算法代码放置的地方，模板已经创建了一个示例的库目标 **ALGO**，其中包含了一个示例的算法类 Adder。

   exp 目录用于算法开发人员对算法代码进行实验和测试，其中已经包含了一个可执行构建目标 **EXP**。

2. 对于应用开发人员：

   app 目录存放最终发布的应用程序源码，其中已经包含了一个可执行构建目标 **APP**。

   test 目录用于应用开发人员编写单元测试，其中已经包含了一个基于 Boost.Test 的 **Test_1** 可执行构建目标。

3. 其它

   根目录中还有一个 **\_PCH** 构建目标，它用于提供预编译头文件的功能，此外也负责生成项目版本头文件，要引用这个头文件，在依赖项目中编写：

   ```c++
   #include "version.hpp"
   ```

   doc 目录中有一个 **DOC** 构建目标，它使用 Doxygen 生成项目文档，已经配置妥当。

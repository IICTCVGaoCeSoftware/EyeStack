# 配置 Eyestack 开发环境

> 本文写作日期：2022/3/31
> 修订：2022/4/18

Eyestack 是一套完整的用于机器视觉工业缺陷检测应用开发的解决方案 —— 不仅仅是一些底层的库和依赖，也包括 IDE、文档生成等高级工具的搭配使用。

这里列出了使其完整功能得以发挥的推荐配置，这些组合已经得到实践验证有效：

- 程序包
  - Qt 5.15.2
  - OpenCV 4.5.3
  - Boost 1.76.0

- 开发工具
  - Git 2.34.1
  - Qt Creator 5.0.1
  - CMake 3.21.3
  - Visual Studio 2019 (vc142)
  - Doxygen 1.9.3
  - Graphviz 2.50.0
  - ISPC 1.16.1
  - Python 3.9.7

如果遇到困难，本文其它几节内容可能会有所帮助，当你把上面该装的都装好了之后，就可以安装构建 Eyestack 了：

1. 使用 Git 从仓库的主分支克隆 Eyestack 最新的发行版本。
2. 复制根目录下的 env.cmake.example 为 env.cmake，根据自己电脑上的环境配置修改文件内容。
3. 在 Qt Creator 中打开 Eyestack 根目录中的 CMakeLists.txt，如果项目加载成功，则表明环境已就绪。
4. 构建 Eyestack 的 Debug 版本和 Release 版本。

---

## Qt 5.15.2 安装配置教程

https://www.notion.so/Qt-5-15-2-906410884a6f4b84ab152a23906f354b

## OpenCV 4.5.3 安装配置教程

https://www.notion.so/OpenCV-4-5-3-8c2977c8c1b241a5aebc88ec8b1d5f14

## Boost 1.76.0 安装配置要点

1. 下载 Boost 并解压之后，在文件夹中打开命令行，运行目录下的 bootstrap 脚本
2. bootstrap 脚本会使用你电脑上的编译器构建 b2 可执行程序
3. 在命令行中直接执行 `b2` 命令，构建静态链接库版本
4. 再执行 `b2 link=shared` 构建动态链接库版本
5. 如果在目录下出现了 stage 文件夹，则已配置成功

## ISPC 1.16.1 安装配置要点

1. 官网链接：https://ispc.github.io/
2. 下载安装好之后，别忘了把 ispc 可执行文件所在目录加入 PATH 环境变量
3. 执行 `ispc --version` 如果有下面这样的输出则安装成功

  ```
  >ispc --version
  Intel(r) Implicit SPMD Program Compiler (Intel(r) ISPC), 1.16.1 (build date Jul 15 2021, LLVM 12.0.1)
  Supported Visual Studio versions: Visual Studio 2015 and later.    
  ```

## Python 3.9.7 安装配置要点

当你安装完 Python 解释器之后，你需要保证 python 这个命令在环境变量中可以找到。此外，你还需要使用 pip 安装一个包“mako”，才可以使用框架的模板项目生成功能：

```
pip install mako
```

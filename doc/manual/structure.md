# 项目结构 {#manual-structure}

## 目录结构

- README.md

  项目的自述文件和文档入口。

- CMakeLists.txt

  顶级 CMake 构建清单文件。

- deps.cmake

  公开依赖脚本，里面编写了 Eyestack 和上层应用共同依赖的底层程序库。

- env.cmake.example

  环境解耦脚本示例，包含与系统环境相关的配置。

- .gitignore; .gitattributes; .gitmodules

  Git 忽略文件、Git LFS 配置文件、Git Submodule 配置文件。

- module/

  按模块组织的源代码目录，每个模块一个子目录。

  因为头文件包含路径的要求，每个模块的公开头文件都放置在 inc 目录的对应模块路径下，例如，模块 Base 的头文件在 `inc/Eyestack/Base` 目录下。这种方式可以避免不同项目的头文件直接在包含时发生名称冲突。

  源文件通常直接放在对应模块下，如果模块下还有其它杂七杂八的文件和目录，有时也放置在模块下的 src 目录以分隔开。

- doc/

  项目文档，包括直接阅读的文档和文档生成目标。

- template/

  上层项目模板

- test/

  单元测试，对应 module 目录，每个模块一个子目录。

- sample/

  使用样例，在框架中一起构建。

- 3rdparty/

  第三方库的 CMake 项目包装。

## 逻辑结构

![](structure.png)

上面是 Eyestack 框架在应用开发时体现的逻辑结构栈图，它大致与项目的结构相对应。

## 框架模块

- Base

  基础支持模块，包含异常类、工具函数等。

- Asset

  静态资源模块，包含图标、音效等资源文件。基于 qrc 机制，在应用启动时直接加载到内存。

- Design

  独立的界面组件模块，每个组件都是一个继承 QWidget 的类。

- Framework

  框架服务模块。

- Gencom

  通用组件模块。通用组件指直接封装 Eyestack 必须依赖的底层库而得到的组件。

- Com\_\*

  其它第三方可选组件模块以“Com\_”开头，例如 Com_MVS 为封装海康威视工业相机 SDK 得到的上层组件。

在向 Eyestack 添加模块时遵循以下规范：

- 每个模块对应 `Eyestack::` 命名空间下的一个子命名空间。
- 模块组织采用扁平化的单层目录结构，模块之间的依赖应是单向的。

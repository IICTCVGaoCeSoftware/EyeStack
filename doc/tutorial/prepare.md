# 1. 准备项目模板 {#introduce-Prepare}

> 本文写作日期：2022/3/14

Eyestack 提供了方便上层项目开发的模板，这些模板被组织为 CMake 自定义构建目标，放在 template 文件夹下：

![](template_targets.png)

Eyestack 默认不会构建模板目标，因此需手动构建，右键 Tpl_Traditional，在弹出菜单中手动构建模板。当构建完成后，到项目构建目录中的就可以找到构建出的模板项目文件夹 `template/Traditional`。

> 如果你不知道你的构建目录在哪里，可以在 Qt Creator 左侧的项目标签页中查看：
>
> ![](build_directory.png)

你应该已经留意到 Traditional 文件夹里面已经有了一个 `CMakeLists.txt` 文件，没错，Traditional 已经是一个完整的 CMake 项目了。Eyestack 已经根据自己的环境配置好了子项目的环境，你确实可以直接在这里打开 Qt Creator，但是我推荐你将文件夹拷贝一份到其它地方再开始开发。

文件夹名可以随意修改，你可以把它改为你的项目名，拷贝完成后，我们打开根目录下的 CMakeLists.txt 修改构建项目名，在这里，我们改名为 `Getting_Started`：

![](change_project_name.png)

修改完成后，在 Qt Creator 中打开效果如下：

![](getting_started_project.png)

模板根目录中得到 README.md 详细介绍了各个模板的项目结构和使用说明，此处不再赘述。

---

接下来：[向模板项目中添加算法](./Add_Sobel.md)

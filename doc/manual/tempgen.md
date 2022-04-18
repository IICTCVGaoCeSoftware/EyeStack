# tempgen.py - 上层项目模板生成脚本 {#manual-tempgen}

tempgen.py 是一个 Eyestack 使用的用于生成上层项目模板的脚本，它在 CMake 配置时从 template/tempgen.py.in 生成，其基于 mako 模板引擎（[官网](https://www.makotemplates.org/)）。

tempgen.py 只是一个普通的 python 程序，在命令行中传入参数使用：

```
python3 tempgen.py <src_dir> <dst_dir>
```

递归扫描 src_dir 中的所有文件，然后：

1. 如果文件名以“.mako”结尾，则使用 Mako 引擎处理后，在 dst_dir 下的对应目录生成处理后的文件；
2. 如果目录中存在以“.mako.py”结尾的文件，则在所有处理前按字典序执行脚本中的内容，脚本本身不会被拷贝到目录中。
3. 如果文件名以“.makoignore”结尾，则不进行处理，也不进行拷贝。
4. 对于其它情况，直接将文件拷贝到目标目录中对应位置。

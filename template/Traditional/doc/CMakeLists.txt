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

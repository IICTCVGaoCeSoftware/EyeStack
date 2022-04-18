# 引入 Doxygen
find_package(Doxygen REQUIRED dot OPTIONAL_COMPONENTS mscgen dia)

# 配置 Doxygen 变量
set(DOXYGEN_IMAGE_PATH .)         # 设置图片搜索目录
set(DOXYGEN_USE_MATHJAX YES)      # 启用数学公式
set(DOXYGEN_CREATE_SUBDIRS YES)

# 添加文档构建目标
doxygen_add_docs(DOC
  # 主页
  index.md

  # 提取代码中的注释
  ../app
)

# 部署规则
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set_target_properties(DOC PROPERTIES EXCLUDE_FROM_ALL FALSE)
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION doc)
endif()

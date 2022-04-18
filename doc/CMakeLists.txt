# Doxygen
find_package(Doxygen REQUIRED dot OPTIONAL_COMPONENTS mscgen dia)

# 新手引导
file(GLOB _tutorial tutorial/*.md)

# 使用指南
file(GLOB _guide guide/*.md)

# 设计手册
file(GLOB _manual manual/*.md)

# 给 doxygen_add_docs 传参
set(DOXYGEN_IMAGE_PATH .)
set(DOXYGEN_USE_MATHJAX YES)
set(DOXYGEN_CREATE_SUBDIRS YES)
#set(DOXYGEN_GENERATE_TREEVIEW YES)
set(DOXYGEN_HTML_EXTRA_STYLESHEET doxygen-awesome-css/doxygen-awesome.css)

doxygen_add_docs(doc
  # 主页
  README.md

  # 提取代码中的注释
  ../module

  ${_tutorial} ${_guide} ${_manual}
)

# 部署规则
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set_target_properties(doc PROPERTIES EXCLUDE_FROM_ALL FALSE)
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION doc)
endif()

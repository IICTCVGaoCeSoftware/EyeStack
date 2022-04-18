#
# 默认算法目标
#
set(_inc inc/${cmake_vars["TEMPLATE_PROJECT_NAME"]})

add_library(${cmake_vars["TEMPLATE_PROJECT_NAME"]}
  # 接口文件
  ${"${"}_inc}/../${cmake_vars["TEMPLATE_PROJECT_NAME"]}.hpp ${cmake_vars["TEMPLATE_PROJECT_NAME"]}.cpp

  # V1 版本实现
  V1.hpp V1.cpp
)

target_link_libraries(${cmake_vars["TEMPLATE_PROJECT_NAME"]} PUBLIC _PCH Eyestack::Design)

target_include_directories(${cmake_vars["TEMPLATE_PROJECT_NAME"]}
  PUBLIC
    $<BUILD_INTERFACE:${"${"}CMAKE_CURRENT_SOURCE_DIR}/inc>
    $<INSTALL_INTERFACE:inc>

  INTERFACE
    $<BUILD_INTERFACE:${"${"}CMAKE_CURRENT_SOURCE_DIR}>
)

target_precompile_headers(${cmake_vars["TEMPLATE_PROJECT_NAME"]} REUSE_FROM _PCH)

# 动态库导出宏定义
get_target_property(_type ${cmake_vars["TEMPLATE_PROJECT_NAME"]} TYPE)
if (_type STREQUAL "STATIC_LIBRARY")
  target_compile_definitions(${cmake_vars["TEMPLATE_PROJECT_NAME"]} PUBLIC ALGO_EXPORT=)
else()
  target_compile_definitions(${cmake_vars["TEMPLATE_PROJECT_NAME"]}
    INTERFACE ${cmake_vars["TEMPLATE_PROJECT_NAME"]}_EXPORT=Q_DECL_IMPORT
    PRIVATE ${cmake_vars["TEMPLATE_PROJECT_NAME"]}_EXPORT=Q_DECL_EXPORT
  )
endif()

# 部署规则
install(TARGETS ${cmake_vars["TEMPLATE_PROJECT_NAME"]} EXPORT ${"${"}EXPORT_TARGETS})
install(DIRECTORY inc DESTINATION .)

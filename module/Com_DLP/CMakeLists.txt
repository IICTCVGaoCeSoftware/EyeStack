# 官方库
add_subdirectory(official)

#
# 包装组件库
#
set(_inc inc/Eyestack/Com_DLP)

add_library(Com_DLP SHARED
  ${_inc}/ginit.hpp ginit.cpp

  ${_inc}/dlpc_common.hpp dlpc_common.cpp

  ${_inc}/dlpc347x.hpp dlpc347x.cpp

  ${_inc}/dlpc347x_internal_patterns.hpp dlpc347x_internal_patterns.cpp

  ${_inc}/cypress_i2c.hpp cypress_i2c.cpp

  ${_inc}/dlpc347x_samples.hpp dlpc347x_samples.cpp

  ${_inc}/dlpconfigui.h dlpconfigui.cpp dlpconfigui.ui
)

target_include_directories(Com_DLP
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/official/Includes>
    $<INSTALL_INTERFACE:inc>
)

target_link_libraries(Com_DLP PUBLIC Framework Eyestack::Com_DLP_official)


target_compile_definitions(Com_DLP
  INTERFACE EYESTACK_COM_DLP_EXPORT=Q_DECL_IMPORT
  PRIVATE EYESTACK_COM_DLP_EXPORT=Q_DECL_EXPORT
) # 动态库都要加这个预定义宏指令

target_precompile_headers(Com_DLP REUSE_FROM pch)

# 部署规则
 install(TARGETS Com_DLP EXPORT ${EXPORT_TARGETS})
install(DIRECTORY inc DESTINATION .)
 install(DIRECTORY third_party DESTINATION module/Com_DLP)

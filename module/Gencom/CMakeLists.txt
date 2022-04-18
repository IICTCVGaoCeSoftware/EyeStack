# 通用组件库
set(_inc inc/Eyestack/Gencom)

add_library(Gencom SHARED
  ${_inc}/../Gencom.hpp Gencom.cpp

  # 基础功能
  ${_inc}/base.hpp

  # 视频捕获器组件
  ${_inc}/VideoCapture.hpp VideoCapture.cpp

  # 监视器组件
  ${_inc}/Monitor.hpp Monitor.cpp

  # 透视变换组件
  ${_inc}/WarpPerspective.hpp WarpPerspective.cpp

  # 参数管理器组件模板
#  ${_inc}/Paramgr.hpp Paramgr.cpp

  # 图像工作台组件
#  ${_inc}/ImageBench.hpp ImageBench.cpp

  # 背景差分组件
  # ${_inc}/BackgroundSubtractor.hpp
  # BackgroundSubtractor.cpp
  # BackgroundSubtractorConfigUi.ui
)

target_include_directories(Gencom
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    $<INSTALL_INTERFACE:inc>
)

target_link_libraries(Gencom PRIVATE Framework)

target_compile_definitions(Gencom
  INTERFACE EYESTACK_GENCOM_EXPORT=Q_DECL_IMPORT
  PRIVATE EYESTACK_GENCOM_EXPORT=Q_DECL_EXPORT
)

target_precompile_headers(Gencom REUSE_FROM pch)

# 部署规则
install(TARGETS Gencom EXPORT ${EXPORT_TARGETS})
install(DIRECTORY inc DESTINATION .)

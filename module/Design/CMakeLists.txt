# Eyestack 独立UI组件模块

set(_inc inc/Eyestack/Design)

add_library(Design SHARED
  ${_inc}/ImageBench.hpp ImageBench.cpp

  ${_inc}/ImageLabel.hpp ImageLabel.cpp

  ${_inc}/Monitor.hpp Monitor.cpp

  ${_inc}/Paramgr.hpp Paramgr.cpp

  ${_inc}/QuadrangleSelector.hpp QuadrangleSelector.cpp

  ${_inc}/RectangleSelector.hpp RectangleSelector.cpp

  ${_inc}/ListScroll.hpp ListScroll.cpp

  ${_inc}/StyleWindow.hpp StyleWindow.cpp

  ${_inc}/AboutEyestack.hpp AboutEyestack.cpp

  # 进度监视器
  ${_inc}/Progressor.hpp Progressor.cpp
)

target_include_directories(Design
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    $<INSTALL_INTERFACE:inc>
)

target_link_libraries(Design PUBLIC Asset)

target_compile_definitions(Design
  INTERFACE EYESTACK_DESIGN_EXPORT=Q_DECL_IMPORT
  PRIVATE EYESTACK_DESIGN_EXPORT=Q_DECL_EXPORT
)

target_precompile_headers(Design REUSE_FROM pch)

# 部署规则
install(TARGETS Design EXPORT ${EXPORT_TARGETS})
install(DIRECTORY inc DESTINATION .)

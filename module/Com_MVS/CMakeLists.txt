# 官方库
add_subdirectory(official)


#
# 包装组件库
#
set(_inc inc/Eyestack/Com_MVS)

add_library(Com_MVS SHARED
  ${_inc}/ginit.hpp ginit.cpp

  ${_inc}/MvsCamera.hpp MvsCamera.cpp

  ${_inc}/MvsCameraWrapper.hpp MvsCameraWrapper.cpp

  ${_inc}/MvsCameraConfigUi.hpp MvsCameraConfigUi.cpp MvsCameraConfigUi.ui

  ${_inc}/MvsCameraPropertyDialog.hpp MvsCameraPropertyDialog.cpp
  MvsCameraPropertyDialog.ui
)

target_include_directories(Com_MVS
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/official/Includes>
    $<INSTALL_INTERFACE:inc>
)

target_link_libraries(Com_MVS PUBLIC Framework Eyestack::Com_MVS_official)

target_compile_definitions(Com_MVS
  INTERFACE EYESTACK_COM_MVS_EXPORT=Q_DECL_IMPORT
  PRIVATE EYESTACK_COM_MVS_EXPORT=Q_DECL_EXPORT
) # 动态库都要加这个预定义宏指令

target_precompile_headers(Com_MVS REUSE_FROM pch)



# 部署规则
install(TARGETS Com_MVS EXPORT ${EXPORT_TARGETS})
install(DIRECTORY inc DESTINATION .)
install(DIRECTORY official DESTINATION module/Com_MVS)

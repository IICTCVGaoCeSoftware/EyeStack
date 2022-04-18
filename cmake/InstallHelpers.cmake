#==============================================================================
# 这个模块提供了一组以 install_ 开头的函数，用于辅助上层应用项目编写部署规则
#==============================================================================

include(${CMAKE_CURRENT_LIST_DIR}/DeployQt.cmake)



#
# 给项目添加 Eyestack 运行时的部署规则
#
# 用法：
#   install_eyestack_runtime(<target> [MODULE <modules>...])
#
# 说明：
#   target: Eyestack 应用主程序目标
#   modules: Eyestack 模块，例如 Gencom
#
function(install_eyestack_runtime _target)
  cmake_parse_arguments("" "" "" "MODULE" ${ARGN})

  if (_UNPARSED_ARGUMENTS)
    message(WARNING "Unused Arguments: \"${_UNPARSED_ARGUMENTS}\"")
  endif()

  # 安装主程序运行时
  install(TARGETS ${_target} RUNTIME)

  # 部署系统运行时库
  if(CMAKE_BUILD_TYPE EQUAL "Debug")
    set(CMAKE_INSTALL_DEBUG_LIBRARIES TRUE)
  endif()

  set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
  include(InstallRequiredSystemLibraries)

  # 部署 Qt
  deployqt(${_target})

  # 部署 OpenCV
  install(IMPORTED_RUNTIME_ARTIFACTS opencv_world)

  # 部署 Eyestack 框架
  install(IMPORTED_RUNTIME_ARTIFACTS
    Eyestack::Base Eyestack::Asset Eyestack::Design Eyestack::Framework
  )

  # 部署 Eyestack 其它模块
  foreach(_module ${_MODULE})
    cmake_language(CALL "install_eyestack_${_module}_runtime")
  endforeach()
endfunction()



function(install_eyestack_Gencom_runtime)
  install(IMPORTED_RUNTIME_ARTIFACTS Eyestack::Gencom)
endfunction()


#
# 给项目添加第三方库运行时的部署规则
#
# 用法：
#   install_3rdparty_mvs_runtime()
#
# 说明：
#   给项目添加 MVS SDK 的运行时文件部署规则，如果提供了 DESTINATION，则将 dir 作为这些
#   文件的输出目录，否则输出到默认的 bin 目录。
#
function(install_eyestack_Com_MVS_runtime)
  file(GLOB _files ${_runtime_dir}/*)

  install(IMPORTED_RUNTIME_ARTIFACTS Eyestack::Com_MVS)
  install_3rdparty_mvs_runtime()
endfunction()

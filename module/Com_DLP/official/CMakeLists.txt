# 平台检查
if(NOT WIN32)
  message(WARNING "Eyestack::Com_DLP 仅支持 win32 和 win64 平台")
  return()
endif()

if(NOT MSVC)
  message(WARNING "Eyestack::Com_DLP 仅支持 MSVC 生成工具")
  return()
endif()

add_library(Eyestack::Com_DLP_official SHARED IMPORTED GLOBAL)

set(_lib_dir ${CMAKE_CURRENT_LIST_DIR}/Libraries)
set(_runtime_dir ${CMAKE_CURRENT_LIST_DIR}/Runtime)

set_target_properties(Eyestack::Com_DLP_official
  PROPERTIES
    IMPORTED_IMPLIB ${_lib_dir}/cyusbserial.lib
    IMPORTED_LOCATION ${_runtime_dir}/cyusbserial.dll
) # 注意这两个属性名，非常关键

file(GLOB _files ${_runtime_dir}/*)

set(EYESTACK_COM_DLP_RUNTIME_FILES ${_files}
  CACHE STRING "Com_DLP 第三方运行时文件清单。"
)

function(install_3rdparty_dlp_runtime)
  install(FILES ${EYESTACK_COM_DLP_RUNTIME_FILES} TYPE BIN)
endfunction()

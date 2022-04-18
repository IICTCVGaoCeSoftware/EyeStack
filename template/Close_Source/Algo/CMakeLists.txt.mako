cmake_minimum_required(VERSION 3.21)

include(env.cmake OPTIONAL RESULT_VARIABLE _env_cmake_found)
if(NOT _env_cmake_found)
  include(env.cmake.example OPTIONAL NO_POLICY_SCOPE)
endif()
unset(_env_cmake_found)

project(${cmake_vars["TEMPLATE_PROJECT_NAME"]} VERSION 0.0.0.0)

include(deps.cmake)

# 生成版本号头文件
set(_version_hpp ${"${"}CMAKE_BINARY_DIR}/inc/${"${"}PROJECT_NAME}/version.hpp)
configure_file(version.hpp.in ${"${"}_version_hpp})

# 安装导出目标名
set(EXPORT_TARGETS ${"${"}PROJECT_NAME}Targets)

# 设置调试配置后缀
set(CMAKE_DEBUG_POSTFIX "-d")



#
# 预编译头文件目标
#
add_library(_PCH STATIC _pch.hpp _pch.cpp version.hpp.in ${"${"}_version_hpp})

target_link_libraries(_PCH PUBLIC Eyestack::Base)

target_include_directories(_PCH PUBLIC
  $<BUILD_INTERFACE:${"${"}CMAKE_BINARY_DIR}/inc>
  $<INSTALL_INTERFACE:inc>
)

target_precompile_headers(_PCH PUBLIC _pch.hpp)

install(TARGETS _PCH EXPORT ${"${"}EXPORT_TARGETS})
install(FILES _pch.hpp ${"${"}_version_hpp} DESTINATION inc/${"${"}PROJECT_NAME})



# 源代码
add_subdirectory(algo)

# 文档
add_subdirectory(doc)

# 实验
add_subdirectory(exp EXCLUDE_FROM_ALL)



#
# 部署配置
#
export(EXPORT ${"${"}EXPORT_TARGETS} NAMESPACE ${"${"}PROJECT_NAME}::)

install(EXPORT ${"${"}EXPORT_TARGETS} DESTINATION cmake
  NAMESPACE ${"${"}PROJECT_NAME}::)

include(CMakePackageConfigHelpers)

configure_package_config_file(
  ${"${"}CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
  ${"${"}CMAKE_CURRENT_BINARY_DIR}/${"${"}PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION cmake
)

write_basic_package_version_file(
  ${"${"}CMAKE_CURRENT_BINARY_DIR}/${"${"}PROJECT_NAME}Version.cmake
  COMPATIBILITY AnyNewerVersion
)

install(FILES
  ${"${"}CMAKE_CURRENT_BINARY_DIR}/${"${"}PROJECT_NAME}Config.cmake
  ${"${"}CMAKE_CURRENT_BINARY_DIR}/${"${"}PROJECT_NAME}Version.cmake
  DESTINATION cmake
)

# 发布时同时打包 Debug 版本
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  string(REPLACE "Release" "Debug" _debug_binary_dir
    ${"${"}CMAKE_CURRENT_BINARY_DIR})
  file(APPEND ${"${"}CMAKE_CURRENT_BINARY_DIR}/CPackConfig.cmake
"
set(CPACK_INSTALL_CMAKE_PROJECTS
  \"${"${"}_debug_binary_dir};${"${"}PROJECT_NAME};ALL;/\"
  \"${"${"}CMAKE_BINARY_DIR};${"${"}PROJECT_NAME};ALL;/\"
)
"
  )
endif()

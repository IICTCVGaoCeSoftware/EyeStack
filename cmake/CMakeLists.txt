#
# 就地导出使用方式
#
configure_file(ExportConfig.cmake.in EyestackConfig.cmake)

# 导出Eyestack目标库，命名空间为Eyestack::
export(EXPORT ${EXPORT_TARGETS} NAMESPACE Eyestack::)

#
# 打包部署使用方式
#
include(CPack)
include(CMakePackageConfigHelpers)

# 为上层应用提供配置框架功能
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/InstallConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/install/${PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION cmake
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/install/${PROJECT_NAME}Version.cmake
  COMPATIBILITY AnyNewerVersion
)

install(EXPORT ${EXPORT_TARGETS}
  DESTINATION cmake # 这里的 DESTINATION 是相对安装目录的路径
  NAMESPACE Eyestack::
)

# 部署依赖文件
install(FILES
  deps.cmake InstallHelpers.cmake DeployQt.cmake TargetIspcSources.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/install/${PROJECT_NAME}Config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/install/${PROJECT_NAME}Version.cmake
  DESTINATION cmake
)

configure_file(package_readme.md.in package_readme.md)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/package_readme.md
  DESTINATION . RENAME README.md
)

# 发布时同时打包 Debug 版本
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  string(REPLACE "Release" "Debug" _debug_binary_dir
    ${CMAKE_CURRENT_BINARY_DIR})

  file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/../CPackConfig.cmake
"
set(CPACK_INSTALL_CMAKE_PROJECTS
  \"${_debug_binary_dir}/..;${PROJECT_NAME};ALL;/\"
  \"${CMAKE_CURRENT_BINARY_DIR}/..;${PROJECT_NAME};ALL;/\"
)
"
  )
endif()

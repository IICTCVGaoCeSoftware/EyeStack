# 生成版本号头文件
configure_file(version.hpp.in
  ${CMAKE_CURRENT_BINARY_DIR}/inc/Eyestack/version.hpp
)

# 预编译头文件目标
add_library(pch
  inc/Eyestack/pch.hpp pch.cpp
  version.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/inc/Eyestack/version.hpp
)

target_link_libraries(pch PUBLIC Qt5::Widgets opencv_world)

target_include_directories(pch
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/inc>
    $<INSTALL_INTERFACE:inc>
)

target_precompile_headers(pch PUBLIC <Eyestack/pch.hpp>)

# 部署规则
install(TARGETS pch EXPORT EyestackTargets)
install(DIRECTORY inc DESTINATION .)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/inc/Eyestack/version.hpp
  DESTINATION inc/Eyestack/
)

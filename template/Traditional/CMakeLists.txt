cmake_minimum_required(VERSION 3.21)

include(env.cmake OPTIONAL RESULT_VARIABLE _env_cmake_found)
if(NOT _env_cmake_found)
  include(env.cmake.example OPTIONAL)
endif()

# TODO 在这里修改应用名
project(Traditional VERSION 0.0.0.0)

include(deps.cmake)



# 生成版本号头文件
configure_file(version.hpp.in ${CMAKE_BINARY_DIR}/inc/version.hpp)

# 预编译头文件目标
add_library(_PCH _pch.hpp _pch.cpp
  version.hpp.in ${CMAKE_BINARY_DIR}/inc/version.hpp
)

target_link_libraries(_PCH PUBLIC Eyestack::_PCH)

target_include_directories(_PCH PUBLIC ${CMAKE_BINARY_DIR}/inc)

target_precompile_headers(_PCH PUBLIC _pch.hpp)



# 算法
add_subdirectory(algo)

# 应用
add_subdirectory(app)

# 文档
add_subdirectory(doc)

# 实验
add_subdirectory(exp EXCLUDE_FROM_ALL)

# 测试
add_subdirectory(test EXCLUDE_FROM_ALL)



#
# 部署配置
#
install_eyestack_runtime(APP MODULE Gencom)

get_target_property(_type ALGO TYPE)
if (_type STREQUAL "SHARED_LIBRARY")
  install(TARGETS ALGO RUNTIME)
endif()

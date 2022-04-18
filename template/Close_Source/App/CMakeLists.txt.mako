cmake_minimum_required(VERSION 3.21)

include(env.cmake OPTIONAL RESULT_VARIABLE _env_cmake_found)
if(NOT _env_cmake_found)
  include(env.cmake.example OPTIONAL)
endif()
unset(_env_cmake_found)

project(${cmake_vars["TEMPLATE_PROJECT_NAME"]}App VERSION 0.0.0.0)

include(deps.cmake)

# 生成版本号头文件
set(_version_hpp ${"${"}CMAKE_BINARY_DIR}/inc/version.hpp)
configure_file(version.hpp.in ${"${"}_version_hpp})

# 预编译头文件目标
add_library(_PCH _pch.hpp _pch.cpp version.hpp.in ${"${"}_version_hpp})



#
# 预编译头文件目标
#
target_link_libraries(_PCH PUBLIC Eyestack::Base)

target_include_directories(_PCH PUBLIC ${"${"}CMAKE_BINARY_DIR}/inc)

target_precompile_headers(_PCH PUBLIC _pch.hpp)



# 应用
add_subdirectory(app)

# 文档
add_subdirectory(doc)

# 测试
add_subdirectory(test EXCLUDE_FROM_ALL)

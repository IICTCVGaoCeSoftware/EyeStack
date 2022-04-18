cmake_minimum_required(VERSION 3.21)

include(env.cmake OPTIONAL RESULT_VARIABLE _env_cmake_found)
if(NOT _env_cmake_found)
  include(env.cmake.example OPTIONAL)
endif()

project(Eyestack VERSION 1.0.0.0)

include(cmake/deps.cmake)

# 安装导出目标名
set(EXPORT_TARGETS ${PROJECT_NAME}Targets)

# 设置调试配置后缀
set(CMAKE_DEBUG_POSTFIX "-d")



# 模块
add_subdirectory(module)

# 文档
add_subdirectory(doc)

# 上层项目模板
add_subdirectory(template)

# 测试
add_subdirectory(test EXCLUDE_FROM_ALL)

# 样例
add_subdirectory(sample EXCLUDE_FROM_ALL)

# CMake 辅助脚本
add_subdirectory(cmake)

# Eyestack 搜索目录，按需修改为不同的配置版本
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(Eyestack_DIR "${cmake_vars["CMAKE_BINARY_DIR"].replace("-Release", "-Debug")}/cmake")
else()
  set(Eyestack_DIR "${cmake_vars["CMAKE_BINARY_DIR"].replace("-Debug", "-Release")}/cmake")
endif()

# OpenCV 搜索目录
set(OpenCV_DIR "${cmake_vars["OpenCV_DIR"]}")

# Boost 搜索目录
set(BOOST_ROOT "${cmake_vars["BOOST_ROOT"]}")

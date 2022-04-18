#
# Base 模块测试
#
add_executable(Test_Base Test_Base.hpp Test_Base.cpp)

target_link_libraries(Test_Base
  PRIVATE Base Boost::unit_test_framework
)

target_precompile_headers(Test_Base REUSE_FROM pch)

add_test(Test_Base Test_Base)

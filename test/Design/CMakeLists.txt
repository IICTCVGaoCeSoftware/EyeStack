#
# Design 模块测试
#
add_executable(Test_Design Test_Design.hpp Test_Design.cpp)

target_link_libraries(Test_Design
  PRIVATE Design Boost::unit_test_framework
)

target_precompile_headers(Test_Design REUSE_FROM pch)

add_test(Test_Design Test_Design)

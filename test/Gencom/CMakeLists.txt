#
# 通用组件测试
#
add_executable(Test_Gencom Test_Gencom.hpp Test_Gencom.cpp)

target_link_libraries(Test_Gencom
  PRIVATE Framework Boost::unit_test_framework Gencom
)

target_precompile_headers(Test_Gencom REUSE_FROM pch)

add_test(Test_Gencom Test_Gencom)

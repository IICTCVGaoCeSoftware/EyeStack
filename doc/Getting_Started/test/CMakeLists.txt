#==============================================================================
# 放置所有测试目标的目录
#==============================================================================



enable_testing()

include(CTest)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

if(NOT Boost_USE_STATIC_LIBS)
  add_compile_definitions(BOOST_TEST_DYN_LINK)
endif()



# 测试目标示例
add_executable(Test_1 test_1.cpp)

target_link_libraries(Test_1 PRIVATE _PCH Boost::unit_test_framework)

target_precompile_headers(Test_1 REUSE_FROM _PCH)

add_test(Test_1 Test_1)



# TODO 按需求添加更多测试目标

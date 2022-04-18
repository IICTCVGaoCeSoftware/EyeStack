#
# 启用测试
#
enable_testing()

include(CTest)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

if(NOT Boost_USE_STATIC_LIBS)
  add_compile_definitions(BOOST_TEST_DYN_LINK)
endif()



#
# 测试模块
#
add_subdirectory(Base)
add_subdirectory(Design)
add_subdirectory(Framework)
add_subdirectory(Gencom)
add_subdirectory(Com_MVS)

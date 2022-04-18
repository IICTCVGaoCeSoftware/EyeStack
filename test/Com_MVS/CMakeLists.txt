#
# MVS 组件测试
#
add_executable(Test_Com_MVS Test_Com_MVS.cpp)

target_link_libraries(Test_Com_MVS
  PRIVATE Framework Boost::unit_test_framework Com_MVS
)

target_precompile_headers(Test_Com_MVS REUSE_FROM pch)

add_test(Test_Com_MVS Test_Com_MVS)



#
# MVS 配置界面测试
#
add_executable(Test_Com_MVS_MvsCameraConfigUi
  Test_MvsCameraConfigUi.cpp
)

target_link_libraries(Test_Com_MVS_MvsCameraConfigUi
  PRIVATE Framework Boost::unit_test_framework Com_MVS
)

target_precompile_headers(Test_Com_MVS_MvsCameraConfigUi REUSE_FROM pch)

add_test(Test_Com_MVS_MvsCameraConfigUi Test_Com_MVS_MvsCameraConfigUi)

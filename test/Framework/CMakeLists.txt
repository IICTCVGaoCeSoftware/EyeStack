#
# 主界面测试
#
add_executable(Test_MainWindow Test_MainWindow.hpp Test_MainWindow.cpp)

target_link_libraries(Test_MainWindow
  PRIVATE Framework Boost::unit_test_framework
)

target_precompile_headers(Test_MainWindow REUSE_FROM pch)

add_test(Test_MainWindow Test_MainWindow)



#
# 工作控制测试
#
add_executable(Test_Workctrl Test_Workctrl.hpp Test_Workctrl.cpp)

target_link_libraries(Test_Workctrl
  PRIVATE Framework Boost::unit_test_framework
)

target_precompile_headers(Test_Workctrl REUSE_FROM pch)

add_test(Test_Workctrl Test_Workctrl)



#
# 空应用程序整体测试
#
add_executable(Test_Application Test_Application.hpp Test_Application.cpp)

target_link_libraries(Test_Application
  PRIVATE Framework Boost::unit_test_framework
)

target_precompile_headers(Test_Application REUSE_FROM pch)

add_test(Test_Application Test_Application)



#
# 配置管理器测试
#
add_executable(Test_Configurer Test_Configurer.cpp)

target_link_libraries(Test_Configurer
  PRIVATE Framework Boost::unit_test_framework
)

target_precompile_headers(Test_Configurer REUSE_FROM pch)

add_test(Test_Configurer Test_Configurer)

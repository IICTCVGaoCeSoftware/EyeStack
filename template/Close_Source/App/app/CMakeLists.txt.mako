#
# 应用程序主体代码
#
add_library(APP OBJECT
  Worker.hpp Worker.cpp
)

target_link_libraries(APP PUBLIC _PCH Eyestack::Framework
  ${cmake_vars["TEMPLATE_PROJECT_NAME"]}::${cmake_vars["TEMPLATE_PROJECT_NAME"]}
)

target_include_directories(APP INTERFACE ${"${"}CMAKE_CURRENT_SOURCE_DIR})

target_precompile_headers(APP REUSE_FROM _PCH)



#
# 应用程序可执行文件
#
add_executable(${"${"}PROJECT_NAME}
  main.cpp

  # 添加程序图标
  favicon.rc favicon.ico
)

target_link_libraries(${"${"}PROJECT_NAME} PRIVATE APP)

target_precompile_headers(${"${"}PROJECT_NAME} REUSE_FROM _PCH)

# 部署规则
install(TARGETS ${"${"}PROJECT_NAME})

install_eyestack_runtime(${"${"}PROJECT_NAME})

#==============================================================================
# 应用软件的源代码目录
#==============================================================================

# TODO 按需求在下面添加多个目标

# 应用程序目标示例
add_executable(APP
  # 主函数
  main.hpp main.cpp
)

target_link_libraries(APP
  PRIVATE ALGO Eyestack::Framework Eyestack::Gencom
)

target_precompile_headers(APP REUSE_FROM _PCH)

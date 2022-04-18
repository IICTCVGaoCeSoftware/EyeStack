#==============================================================================
# 算法实验的源代码目录
#==============================================================================

# TODO 按需求在下面添加多个目标

# 算法实验用目标示例
add_executable(EXP
  # 主函数
  main.hpp main.cpp
)

target_link_libraries(EXP
  PRIVATE ALGO Eyestack::Framework
)

target_precompile_headers(EXP REUSE_FROM _PCH)

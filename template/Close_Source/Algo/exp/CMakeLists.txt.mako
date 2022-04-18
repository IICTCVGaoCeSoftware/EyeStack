#
# 算法实验用目标示例
#
add_executable(EXP
  # 主函数
  main.hpp main.cpp
)

target_link_libraries(EXP PRIVATE _PCH Eyestack::Framework ${cmake_vars["TEMPLATE_PROJECT_NAME"]})

target_precompile_headers(EXP REUSE_FROM _PCH)

#
# 一个基本的示例，展示了 Gencom 和 Com_MVS 的用法
#
add_executable(Spl_Basic Basic.hpp Basic.cpp)

target_link_libraries(Spl_Basic PRIVATE Framework Gencom)

target_precompile_headers(Spl_Basic REUSE_FROM pch)

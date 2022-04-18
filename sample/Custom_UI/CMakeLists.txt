#
# UI 定制示例
#
add_executable(Spl_Custom_UI Custom_UI.hpp Custom_UI.cpp)

target_link_libraries(Spl_Custom_UI PRIVATE Framework)

target_precompile_headers(Spl_Custom_UI REUSE_FROM pch)

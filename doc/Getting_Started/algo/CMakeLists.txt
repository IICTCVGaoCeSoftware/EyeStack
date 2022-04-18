#==============================================================================
# 项目所需的所有算法源代码
#==============================================================================

# TODO 按需求在下面添加多个目标

# 算法库目标示例
set(_inc inc/algo)

add_library(ALGO
  ${_inc}/../algo.hpp algo.cpp

  ${_inc}/Adder.hpp Adder.cpp

  ${_inc}/Sobel.hpp Sobel.cpp
)

target_link_libraries(ALGO PUBLIC _PCH Eyestack::Design)

target_include_directories(ALGO PUBLIC inc)

target_precompile_headers(ALGO REUSE_FROM _PCH)

get_target_property(_type ALGO TYPE)
if (_type STREQUAL "STATIC_LIBRARY")
  target_compile_definitions(ALGO PUBLIC ALGO_EXPORT=)
else()
  target_compile_definitions(ALGO
    INTERFACE ALGO_EXPORT=Q_DECL_IMPORT
    PRIVATE ALGO_EXPORT=Q_DECL_EXPORT
  )
endif ()

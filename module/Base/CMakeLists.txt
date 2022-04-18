# 用到了 Boost 的 date_time 模块，因为 Qt 的对应模块线程不安全。
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS date_time)



#
# Eyestack 基础支持模块
#
set(_inc inc/Eyestack/Base)

add_library(Base SHARED # 使用 STATIC 会导致 Base 中全局变量出现菱形继承问题！
  ${_inc}/../Base.hpp Base.cpp

  # 异常系统
  ${_inc}/exc.hpp

  # 实用函数
  ${_inc}/util.hpp util.cpp

  # 运行时分析工具
  ${_inc}/Profiler.hpp Profiler.cpp

  # 日志记录器
  ${_inc}/Logsys.hpp Logsys.cpp
)

target_link_libraries(Base PUBLIC pch PRIVATE Boost::date_time)

target_include_directories(Base
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    $<INSTALL_INTERFACE:inc>
)

target_compile_definitions(Base
  INTERFACE EYESTACK_BASE_EXPORT=Q_DECL_IMPORT
  PRIVATE EYESTACK_BASE_EXPORT=Q_DECL_EXPORT
)

target_precompile_headers(Base REUSE_FROM pch)

# 部署规则
install(TARGETS Base EXPORT ${EXPORT_TARGETS})
install(DIRECTORY inc DESTINATION .)

# 系统框架模块
set(_inc inc/Eyestack/Framework)

add_library(Framework SHARED
  ${_inc}/../Framework.hpp Framework.cpp

  ${_inc}/Application.hpp Application.cpp

  # 配置保存服务
  ${_inc}/Configurer.hpp Configurer.cpp

  # 工作控制服务
  ${_inc}/Workctrl.hpp Workctrl.cpp

  # 主界面服务
  ${_inc}/MainWindow.hpp MainWindow.cpp

  # 通知服务
  ${_inc}/Notifier.hpp Notifier.cpp

  # 后台任务服务
  ${_inc}/Bgtasker.hpp Bgtasker.cpp
)

target_include_directories(Framework
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    $<INSTALL_INTERFACE:inc>
)

target_link_libraries(Framework PUBLIC Design)

target_compile_definitions(Framework
  INTERFACE EYESTACK_FRAMEWORK_EXPORT=Q_DECL_IMPORT
  PRIVATE EYESTACK_FRAMEWORK_EXPORT=Q_DECL_EXPORT
)

target_precompile_headers(Framework REUSE_FROM pch)

# 部署规则
install(TARGETS Framework EXPORT ${EXPORT_TARGETS})
install(DIRECTORY inc DESTINATION .)

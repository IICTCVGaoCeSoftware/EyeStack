# Eyestack 公共静态资源支持模块

set(_inc inc/Eyestack/Asset)

add_library(Asset SHARED
  ${_inc}/../Asset.hpp Asset.cpp asset.qrc
)

target_include_directories(Asset
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    $<INSTALL_INTERFACE:inc>
)

target_link_libraries(Asset PUBLIC Base)

target_precompile_headers(Asset REUSE_FROM pch)

# 部署规则
install(TARGETS Asset EXPORT ${EXPORT_TARGETS})
install(DIRECTORY inc DESTINATION .)

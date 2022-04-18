set(TEMPLATE_PROJECT_NAME "Template" CACHE STRING "模板生成的项目名")

set(_tempgen "${CMAKE_CURRENT_BINARY_DIR}/tempgen.py")
configure_file(tempgen.py.in ${_tempgen})



#
# 传统机器视觉项目模板
#
add_custom_target(Tpl_Traditional
  python ${_tempgen}
    ${CMAKE_CURRENT_SOURCE_DIR}/Traditional
    ${CMAKE_CURRENT_BINARY_DIR}/Traditional
  SOURCES Traditional/README.md
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set_target_properties(Tpl_Traditional PROPERTIES EXCLUDE_FROM_ALL FALSE)
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Traditional
    DESTINATION template)
endif()



#
# 算法-应用闭源式开发项目模板
#
add_custom_target(Tpl_Close_Source
  python ${_tempgen}
    ${CMAKE_CURRENT_SOURCE_DIR}/Close_Source
    ${CMAKE_CURRENT_BINARY_DIR}/Close_Source
  SOURCES Close_Source/README.md
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set_target_properties(Tpl_Close_Source PROPERTIES EXCLUDE_FROM_ALL FALSE)
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Close_Source
    DESTINATION template)
endif()

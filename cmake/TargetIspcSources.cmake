#==============================================================================
# 提供了一个 target_ispc_sources 函数，用于给目标添加 ISPC 源文件及相应的构建规则
#==============================================================================

# ISPC 默认目标指令集
set(ISPC_DEFAULT_INSTRUCTION_SET avx2-i32x8
  CACHE STRING "Default --target argument for ISPC compiler.")

# ISPC 默认目标架构
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(ISPC_DEFAULT_ARCHITECTURE x86
    CACHE STRING "Default --arch argument for ISPC compiler.")
else()
  set(ISPC_DEFAULT_ARCHITECTURE x86-64
    CACHE STRING "Default --arch argument for ISPC compiler.")
endif()

# ISPC 默认任务系统运行时
set(ISPC_DEFAULT_TASKSYS "${CMAKE_CURRENT_LIST_DIR}/tasksys.cpp"
  CACHE PATH "Default task support cpp file for ISPC compiler.")



#
# 给目标添加 ISPC 源文件
#
# 用法：
#   target_ispc_sources(<target> [source1] [source2 ...]
#     [INSTRUCTION_SET <name>] [ARCHITECTURE <arch>])
#
# 说明：
#   第一个参数 <target> 为使用 add_executable、add_library 等添加的目标名，其后
#   跟随若干个 ISPC 源文件路径，函数将自动给这些文件添加编译规则，并生成可供 C/C++
#   包含的头文件。如果路径是相对路径，则包含头文件为其后加 ".h" 后缀（如 a/b/c.ispc
#   对应包含 a/b.ispc.h），如果是绝对路径，则只是 ISPC 源文件名后加 ".h" 后缀（如
#   C:/a.ispc 对应包含 a.ispc.h）。
#
#   INSTRUCTION_SET 后的参数会被用于 ISPC 的 "--target"，指明编译目标指令集。
#
#   ARCHITECTURE 后的参数被用于 ISPC 的 "--arch"，指明编译目标架构。
#
#   函数会根据 CMake 的目标操作系统自动生成 ISPC 的 "--target-os" 参数。
#
function(target_ispc_sources _target)
  # 解析 INSTRUCTION_SET、ARCHITECTURE 两个单参数到对应变量里
  cmake_parse_arguments(
    "" "USE_TASKSYS" "INSTRUCTION_SET;ARCHITECTURE" ""
    ${ARGN}
  )

  # 两个参数都是可选的，如果没有提供，就用默认值
  if(_INSTRUCTION_SET)
    set(_instruction_set ${INSTRUCTION_SET})
  else()
    set(_instruction_set ${ISPC_DEFAULT_INSTRUCTION_SET})
  endif()
  if(_ARCHITECTURE)
    set(_architecture ${ARCHITECTURE})
  else()
    set(_architecture ${ISPC_DEFAULT_ARCHITECTURE})
  endif()

  # 根据 CMake 环境变量设置 --target-os 参数
  if(WIN32)
    set(_target_os "windows")
  else()
    set(_target_os "linux")
  endif()

  # 取出目标的源码绝对路径到变量 _source_dir 里
  get_target_property(_source_dir ${_target} SOURCE_DIR)

  # 在目标的构建目录下创建 ISPC 编译输出目录（变量 _binary_dir）
  get_target_property(_binary_dir ${_target} BINARY_DIR)
  cmake_path(APPEND _binary_dir "${_target}.ispc")
  file(MAKE_DIRECTORY ${_binary_dir})
  
  # 将输出目录添加到目标的包含目录中
  target_include_directories(${_target} PRIVATE ${_binary_dir})

  # 给参数表剩下的代码文件添加编译规则
  foreach(_source ${_UNPARSED_ARGUMENTS})
    # 将 _source 转为绝对路径
    cmake_path(IS_RELATIVE _source _is_relative)
    if(_is_relative)
      cmake_path(ABSOLUTE_PATH _source BASE_DIRECTORY ${_source_dir}
        NORMALIZE)
    endif()

    # 打印状态消息
    message(STATUS "Adding ISPC source \"${_source}\" to ${_target}.")

    cmake_path(IS_PREFIX _source_dir ${_source} _is_prefix)
    if(_is_prefix)
      # 如果源文件路径在源码目录下，则输出到 _binary_dir 的相对路径下
      cmake_path(RELATIVE_PATH _source BASE_DIRECTORY ${_source_dir}
        OUTPUT_VARIABLE _binary)
      cmake_path(ABSOLUTE_PATH _binary BASE_DIRECTORY ${_binary_dir})

    else()
      # 如果源文件不在源码目录下，则输出到 _binary_dir 下的同名前缀
      cmake_path(GET _source FILENAME _binary)
      cmake_path(ABSOLUTE_PATH _binary BASE_DIRECTORY ${_binary_dir})

    endif()

    # 添加自定义命令
    set(_obj_path "${_binary}${CMAKE_CXX_OUTPUT_EXTENSION}")
    set(_h_path "${_binary}.h")
    message(VERBOSE "ispc ${_source} -o ${_obj_path} -h ${_h_path}"
      "--target=${_instruction_set} --arch=${_architecture}"
      "--target-os=${_target_os}")
    add_custom_command(OUTPUT ${_h_path} ${_obj_path}
      COMMAND ispc ${_source} -o ${_obj_path} -h ${_h_path}
        --target=${_instruction_set} --arch=${_architecture}
        --target-os=${_target_os}
      DEPENDS ${_source}
      # COMMENT "ispc ${_source} -o ${_obj_path} -h ${_h_path}"
      #   "--target=${_instruction_set} --arch=${_architecture}"
      #   "--target-os=${_target_os}"
    )

    # 向目标添加源文件
    target_sources(${_target} PRIVATE ${_obj_path} ${_h_path} ${_source})
  endforeach()

  # 给目标添加任务运行时支持源文件
  if (USE_TASKSYS)
    target_sources(${_target} PRIVATE ${ISPC_DEFAULT_TASKSYS})
  endif()
endfunction()

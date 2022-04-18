#==============================================================================
# 这个模块添加了一个 deployqt 函数，用法：
#
# deployqt(<target> [destination])
#
#   对 target 添加安装规则，使其在打包时自动执行 *deployqt 收集所需的 Qt dll 打进包里；
#   如果没有指定 destination，则使用默认的相对路径 "bin"。
#==============================================================================

cmake_minimum_required(VERSION 3.21)



# Windows 平台使用 windeployqt
if(WIN32)
  function(deployqt _target)
    # 如果传了多余参数，就安装到多余参数下
    if(${ARGC} EQUAL 1)
      set(_install_dst bin)
    elseif(${ARGC} EQUAL 2)
      set(_install_dst ${ARGV1})
    else()
      message(FATAL_ERROR "deployqt 仅接受1个或2个参数，但传入了 ${ARGC} 个。")
    endif()

    # 安装脚本文件
    get_target_property(_binary_dir ${_target} BINARY_DIR)
    set(_script "${_binary_dir}/${_target}_deployqt.cmake")

    # CPack 打包期读取不到 CACHE 中的变量值！
    get_filename_component(_qt_bin_dir ${QT_QMAKE_EXECUTABLE} DIRECTORY)

    # 为目标创建一个安装脚本文件，使用 execute_process 可以使文件并发拷贝
    file(GENERATE OUTPUT ${_script} CONTENT "
      # 传入一些变量值
      set(_target_file \"$<TARGET_FILE:${_target}>\")
      set(_install_dir \"\${CMAKE_INSTALL_PREFIX}/${_install_dst}\")
      set(_qt_bin_dir \"${_qt_bin_dir}\")

      # 执行 windeployqt
      message(\"DeployQt: 为 ${_target} 执行 windeployqt\")
      execute_process(COMMAND windeployqt
          --dry-run --list mapping
          $<IF:$<CONFIG:Debug>,--debug,--release>
          --no-compiler-runtime
          --no-angle
          --no-opengl-sw
          --no-translations
          \${_target_file}
        WORKING_DIRECTORY \${_qt_bin_dir}
        OUTPUT_VARIABLE _output
        OUTPUT_STRIP_TRAILING_WHITESPACE
      )

      # 拷贝 windeployqt 的文件
      message(\"DeployQt: 拷贝 windeployqt 输出文件\")
      separate_arguments(_files WINDOWS_COMMAND \${_output})
      while(_files)
        list(GET _files 0 _src)
        list(GET _files 1 _dst)
        get_filename_component(_dst \${_dst} DIRECTORY)
        file(COPY \${_src} DESTINATION \"\${_install_dir}/\${_dst}\")
        list(REMOVE_AT _files 0 1)
      endwhile()
    ")

    # 注册安装脚本文件
    install(SCRIPT ${_script})
  endfunction()



else()
  message(FATAL_ERROR "不支持的平台。")



endif()

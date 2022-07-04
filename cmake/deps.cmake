# 语言标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)



# MSVC 配置
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")



# Qt5
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

add_compile_definitions(
  # 禁用从 Qt 5.15.0 以来的弃用函数
  QT_DISABLE_DEPRECATED_BEFORE=0x050f00
)

# OpenCV
find_package(OpenCV REQUIRED)

# PCL 1.10
find_package(PCL 1.10 REQUIRED)
find_package(VTK REQUIRED)
find_package(Boost REQUIRED)

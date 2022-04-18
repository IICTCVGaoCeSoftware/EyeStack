#pragma once

namespace Eyestack::Asset {

/**
 * @brief Aseet 模块必须初始化以后才能使用其中的资源文件
 */
void
ginit();

/**
 * @brief 方便使用 icon 目录资源下图标的函数
 * @param path 相对 :/icon/ 的文件路径
 * @return QIcon 图标对象
 */
inline QIcon
icon(const QString& path)
{
  return QIcon(":/icon/" + path);
}

/**
 * @brief 方便使用 FontAwesome 图标的函数
 * @param name FontAwesome 图标名
 * @return QIcon 图标对象
 */
inline QIcon
faicon(const QString& name)
{
  return QIcon(":/icon/fa/" + name + ".svg");
}

}

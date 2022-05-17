#pragma once

namespace Eyestack::Com_DLP {

/**
 * @brief 模块在使用前必须在 Application 构建后进行全局初始化
 */
void
ginit();

#ifdef _DEBUG

/**
 * @brief 调试版本下检查模块是否被初始化的函数
 */
void
ginit_required();

#else

/**
 * @brief 发布版本下不检查模块初始化，实现为内联空函数
 */
inline void
ginit_required()
{}

#endif

/**
 * @brief 用于在调试版本下检查模块是否被初始化的基类
 *
 * 在发布版本下为空实现，不检查初始化。
 */
class GinitRequired
{
#ifdef _DEBUG
public:
  GinitRequired();
#endif
};

}

#include <Eyestack/Design/Paramgr.hpp>

namespace Adder {

namespace esd = Eyestack::Design;

/**
 * @brief 整数加法器类
 *
 * 这段注释同样展示了 Doxygen 的用法。
 */
class ALGO_EXPORT IntAdder
{
public:
  IntAdder();

public:
  /**
   * @brief 获取算法的参数表
   */
  esd::Paramgr::Table& param_table() { return _paramTable; }

  /**
   * @brief 做加法，计算结果
   * @param b 被加数
   * @return 求和结果
   */
  int calc(int b);

private:
  esd::Paramgr::Table _paramTable;
  esd::paramgr::Int _param{ "param", "示例参数", 0 };
  esd::paramgr::Bool _logen{ "logen", "是否启用日志输出" };
};

}

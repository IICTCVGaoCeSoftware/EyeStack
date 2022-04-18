#include "algo/Adder.hpp"
#include <Eyestack/Base.hpp>

using namespace Eyestack::Base;

namespace Adder {

IntAdder::IntAdder()
{
  _paramTable.register_param(_param);
  _paramTable.register_param(_logen);
}

static QLoggingCategory sLogcat("Adder 算法");

int
IntAdder::calc(int b)
{
  // 创建一个 Logger 以使用异步日志功能
  Logger logger;

  if (!_logen)
    logger.set_thread_pool(nullptr); // 运行时禁用输出

  // 使用共享指针以使用 Logsys 的异步日志功能
  Profiler prof;

  {
    // 使用 Scoper 计时语句块的时间开销
    Profiler::Scoper scoper(prof, "日志开销");

    // 使用 QtDebug 打印日志条目
    qDebug(sLogcat) << "这行日志消息只会在 Debug 编译下打印";
    qWarning(sLogcat) << "打印一条报警消息";
  }

  // 异步记录 Profiler 对象
  logger.profiler("calc.prof") << prof;

  // 异步记录一张图片（OpenCV Mat 格式）
  logger.image("test.png") << cv::Mat(100, 100, CV_8UC1);

  return b + _param;
}

}

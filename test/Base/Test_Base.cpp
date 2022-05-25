#include "Test_Base.hpp"

#include <boost/thread/thread.hpp>

#define BOOST_TEST_MODULE Test_Base
#include <boost/test/unit_test.hpp>

#include "Eyestack/Base.hpp"

using namespace Eyestack::Base;

// Profiler 功能测试
BOOST_AUTO_TEST_CASE(test_Profiler_1)
{
  static const char* strs[] = { "a", "b", "c", "d" };
  static const auto strsLen = sizeof(strs) / sizeof(const char*);

  Profiler pro;

  // 测试使用 Scoper 分析记录
  for (int i = 0; i < strsLen; ++i) {
    Profiler::Scoper(pro, strs[i]);
  }

  BOOST_TEST(pro.entries().size() == strsLen * 2);

  BOOST_TEST_MESSAGE("Scoper tested.");

  // 测试 Profiler 的序列化功能
  QByteArray buf;

  {
    QDataStream ds(&buf, QIODevice::WriteOnly);
    ds << pro;
  }

  {
    QDataStream ds(&buf, QIODevice::ReadOnly);

    Profiler newPro;
    ds >> newPro;

    auto& entries = pro.entries();
    auto& newEntries = newPro.entries();

    BOOST_TEST(entries.size() == newEntries.size());

    for (auto i = 0; i < entries.size(); ++i) {
      {
        auto timeEqual = entries[i]._time == newEntries[i]._time;
        BOOST_TEST(timeEqual);
      }

      {
        auto tagEqual = QString(entries[i]._tag) == newEntries[i]._tag;
        BOOST_TEST(tagEqual);
      }
    }
  }

  BOOST_TEST_MESSAGE("Serialization tested.");
}

// Logsys 功能测试
BOOST_AUTO_TEST_CASE(test_Logsys)
{
  Logsys logsys("log/", "log.old/");

  Logger logger;

  auto prof = QSharedPointer<Profiler>::create();
  {
    Profiler::Scoper scoper(*prof, "scope");

    QByteArray bytes("123abc");
    auto bytesLogger = logger.bytes("bytes");
    bytesLogger << bytes;

    cv::Mat mat(100, 100, CV_8UC1);
    logger.image("img.png") << mat;

    bytesLogger << bytes;
  }

  logger.profiler("prof") << prof;
}

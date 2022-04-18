#define BOOST_TEST_MODULE Test_1
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_1)
{
  BOOST_TEST(true);
  BOOST_TEST_MESSAGE("hello, test!");
}

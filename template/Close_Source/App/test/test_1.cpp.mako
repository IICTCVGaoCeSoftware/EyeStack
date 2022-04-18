#define BOOST_TEST_MODULE Test_1
#include <boost/test/unit_test.hpp>

#include "Worker.hpp"

class Skeleton : public ${cmake_vars["TEMPLATE_PROJECT_NAME"]}::${cmake_vars["TEMPLATE_PROJECT_NAME"]}
{
public:
  // TODO
};

BOOST_AUTO_TEST_CASE(test_1)
{
  esf::Application app(0, nullptr);

  auto worker = new Worker();
  worker->_algo = std::make_unique<Skeleton>();
  app.reg_worker(worker);

  BOOST_CHECK(app.exec() == 0);
}

#include "Worker.hpp"

int
main(int argc, char* argv[])
{
  esf::Application app(argc, argv);

  auto worker = new Worker();
  worker->_algo = ${cmake_vars["TEMPLATE_PROJECT_NAME"]}::create();
  app.reg_worker(worker);

  return app.exec();
}

#pragma once

#include <Eyestack/Framework.hpp>
#include <${cmake_vars["TEMPLATE_PROJECT_NAME"]}.hpp>

namespace esf = Eyestack::Framework;

class Worker : public esf::Worker
{
  Q_OBJECT

  using _S = esf::Worker;

public:
  std::unique_ptr<${cmake_vars["TEMPLATE_PROJECT_NAME"]}::${cmake_vars["TEMPLATE_PROJECT_NAME"]}> _algo;

  // Worker interface
protected:
  virtual void work() override;
};

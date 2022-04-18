#pragma once

#include <Eyestack/Framework.hpp>

class MyMdiSubUi : public Eyestack::Framework::MdiSubUi
{
public:
  MyMdiSubUi();

public:
  bool _hasConfigUi, _hasRunningUi;
  QPushButton _configUi, _runningUi;

  // MdiSubUi interface
public:
  virtual void switch_conftime() override;
  virtual void switch_runtime() override;
};

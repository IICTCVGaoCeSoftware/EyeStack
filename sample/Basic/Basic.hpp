#pragma once

#include <Eyestack/Framework.hpp>
#include <Eyestack/Gencom/Monitor.hpp>
#include <Eyestack/Gencom/VideoCapture.hpp>
#include <Eyestack/Gencom/WarpPerspective.hpp>

using namespace Eyestack;

class Worker : public Eyestack::Framework::Worker
{
public:
  Gencom::VideoCapture vc{ *this };
  Gencom::WarpPerspective wp{ *this };
  Gencom::Monitor mo{ *this };

public:
  Worker();

  // Worker interface
private:
  virtual void work() override;
};

void
example_task(Framework::TaskEntry& te);

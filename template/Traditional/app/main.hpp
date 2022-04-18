#pragma once

#include <Eyestack/Framework.hpp>
#include <Eyestack/Gencom.hpp>

namespace es = Eyestack;
namespace esf = Eyestack::Framework;
namespace esg = Eyestack::Gencom;

class Worker : public esf::Worker
{
public:
  esg::VideoCapture _vc{ *this };

  // Worker interface
protected:
  virtual void work() override;
};

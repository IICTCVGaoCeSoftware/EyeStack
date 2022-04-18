#include "Test_Workctrl.hpp"

#include "Eyestack/Framework/Workctrl.hpp"

BOOST_AUTO_TEST_CASE(case_1)
{
  Eyestack::Framework::Workctrl::Default workctrl;

  workctrl.start_worker(123);

  workctrl.pause_worker();

  workctrl.stop_worker();
}

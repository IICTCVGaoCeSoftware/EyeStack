#define BOOST_TEST_MODULE Test_Application
#include <boost/test/unit_test.hpp>

#include "Eyestack/Framework.hpp"

using namespace Eyestack::Framework;

BOOST_AUTO_TEST_CASE(case_1)
{
  Application app(0, nullptr);

  Configurer configurer;

  class Config1 : public Configurer::Config
  {
    // Configurer::Config interface
  public:
    virtual uint64_t uuid() override { return 1; }
    virtual void reset_config() noexcept override {}
    virtual void dump_config(QDataStream& ds) noexcept(false) override
    {
      throw std::exception();
    }
    virtual void load_config(QDataStream& ds) noexcept(false) override
    {
      throw Eyestack::Base::exc::LoadingError("wtf");
    }
  } config1;

  class Config2 : public Configurer::Config
  {
  public:
    bool _flag{ true };

    // Configurer::Config interface
  public:
    virtual uint64_t uuid() override { return 2; }
    virtual void reset_config() noexcept override {}
    virtual void dump_config(QDataStream& ds) noexcept(false) override { ds << _flag; }
    virtual void load_config(QDataStream& ds) noexcept(false) override { ds >> _flag; }
  } config2;

  configurer.register_config(config1);
  configurer.register_config(config2);

  configurer.dump_configs("esconfig");
  config2._flag = false;

  configurer.load_configs("esconfig");

  BOOST_TEST(config2._flag == true);
}

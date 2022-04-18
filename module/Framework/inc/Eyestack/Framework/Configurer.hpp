#pragma once

#include "Eyestack/Base.hpp"

namespace Eyestack::Framework {

/**
 * @brief 配置保存服务类
 */
class Configurer
{
  using _T = Configurer;

public:
  class Config;

public:
  /**
   * @brief 注册配置对象
   *
   * @param uuid 配置对象的唯一标识符
   * @param config 配置对象
   */
  void register_config(uint64_t uuid, Config& config);

  /**
   * @brief 重置所有配置
   */
  void reset_configs() noexcept;

  /**
   * @brief 保存配置到文件
   * @param path 文件路径
   */
  void dump_configs(const QString& path) noexcept;

  /**
   * @brief 从文件加载配置
   * @param path 文件路径
   */
  void load_configs(const QString& path) noexcept;

private:
  QMap<uint64_t, Config*> _configs;
};

/**
 * @brief 配置序列化功能的接口类
 */
class Configurer::Config
{
public:
  virtual ~Config() = default;

public:
  /**
   * @brief 重置组件的配置，不允许抛出异常
   */
  virtual void reset_config() noexcept = 0;

  /**
   * @brief 序列化组件的配置
   * @param ds 已打开的输出数据流
   * @throws 如果失败则抛出异常
   */
  virtual void dump_config(QDataStream& ds) noexcept(false) = 0;

  /**
   * @brief 反序列化组件的配置。该方法的子类实现必须保证在任何可能的执行分支
   * 中从流中读出的数据与起初序列化时写入的数据长度相同。
   * @param ds 已打开的输入数据流
   * @throws 如果失败则抛出异常
   */
  virtual void load_config(QDataStream& ds) noexcept(false) = 0;

public:
  class Default;
  class Functional;
};

class Configurer::Config::Default : public Config
{
  // Config interface
public:
  virtual void reset_config() noexcept override {}
  virtual void dump_config(QDataStream& ds) noexcept(false) override {}
  virtual void load_config(QDataStream& ds) noexcept(false) override {}
};

class Configurer::Config::Functional : public Config
{
public:
  std::function<void()> _reset;
  std::function<void(QDataStream& ds)> _dump;
  std::function<void(QDataStream& ds)> _load;

  // Config interface
public:
  virtual void reset_config() noexcept override { _reset(); }
  virtual void dump_config(QDataStream& ds) noexcept(false) override
  {
    _dump(ds);
  }
  virtual void load_config(QDataStream& ds) noexcept(false) override
  {
    _load(ds);
  }
};

}

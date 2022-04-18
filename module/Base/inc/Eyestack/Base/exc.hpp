#pragma once

namespace Eyestack::Base::exc {

/**
 * @brief 本项目所有异常的基类，为与标准库异常系统兼容，继承了 std::exception，
 * 所以可以用 std::exception 捕获到。
 */
class Exception : public std::exception
{
public:
  using std::exception::exception;

public:
  /**
   * @brief 返回一个人类可读的、描述具体异常信息的字符串
   */
  virtual QString repr() const noexcept { return what(); }

  // exception interface
public:
  virtual const char* what() const override { return typeid(*this).name(); }
};

/**
 * @brief 接口未实现时抛出的异常，常用于 inline-throw 模式
 */
class NotImplementedError : public Exception
{
public:
  using Exception::Exception;
};

/**
 * @brief 运行时异常，可用于运行时方便地构造异常信息
 */
class RuntimeError
  : public QString
  , public Exception
{
public:
  RuntimeError(QString msg)
    : QString(std::move(msg))
  {}

  // Exception interface
public:
  virtual QString repr() const noexcept override { return *this; }
};

/**
 * @brief 系统初始化过程中抛出的异常，通常都是致命的、不可恢复的
 */
class InitializationError : public RuntimeError
{
public:
  using RuntimeError::RuntimeError;
};

/**
 * @brief 序列化失败时抛出的异常
 */
class DumpingError : public RuntimeError
{
public:
  using RuntimeError::RuntimeError;

  // Exception interface
public:
  virtual QString repr() const noexcept override
  {
    return QObject::tr("存储错误：") + *this;
  }
};

/**
 * @brief 反序列化失败时抛出的异常
 */
class LoadingError : public RuntimeError
{
public:
  using RuntimeError::RuntimeError;

  // Exception interface
public:
  virtual QString repr() const noexcept override
  {
    return QObject::tr("加载错误：") + *this;
  }
};

/**
 * @brief 索引错误，例如数组越界、哈希表的键不存在等等。
 */
class IndexError : public RuntimeError
{
public:
  using RuntimeError::RuntimeError;

  // Exception interface
public:
  virtual QString repr() const noexcept override
  {
    return QObject::tr("索引错误：") + *this;
  }
};

/**
 * @brief 无效路径错误，例如文件不存在、希望路径是文件夹结果是文件等等。
 */
class InvalidPathError : public RuntimeError
{
public:
  using RuntimeError::RuntimeError;

  // Exception interface
public:
  virtual QString repr() const noexcept override
  {
    return QObject::tr("无效路径：") + *this;
  }
};

}

#pragma once

#include "Eyestack/Base.hpp"

namespace Eyestack::Design {

/**
 * @brief 无系统边框的、使用自定样式标题栏的窗口组件
 */
class EYESTACK_DESIGN_EXPORT StyleWindow : public QWidget
{
  using _T = StyleWindow;
  using _S = QWidget;

public:
  /**
   * @brief 标题栏绘制组件
   */
  class TitleBar
    : public QWidget
    , public QStyleOptionTitleBar
  {
    using _T = TitleBar;
    using _S = QWidget;
    using _S1 = QStyleOptionTitleBar;

  public:
    TitleBar(QWidget* parent = nullptr)
      : _S(parent)
    {}

    // QWidget interface
  protected:
    virtual void paintEvent(QPaintEvent* event) override;
  };

public:
  /**
   * @brief 使用 QWidget 实现的标题栏对象
   */
  TitleBar _titleBar;

public:
  StyleWindow(QWidget* parent = nullptr);

public:
  /**
   * @brief 设置窗口主体，原 QWidget 对象（如果有）的父对象仍为 StyleWindow
   * @param body 新内容区组件，其父对象将被设置为 StyleWindow
   */
  void set_body(QWidget* body)
  {
    auto old = _layout.takeAt(1);
    if (old)
      delete old; // 删除 QLayoutItem 不会删除所管理的 QWidget
    if (body)
      _layout.addWidget(body);
  }

  /**
   * @brief 获取内容区组件
   * @return 如果 StyleWindow 还没有设置内容，则返回 nullptr
   */
  QWidget* get_body()
  {
    auto item = _layout.itemAt(1);
    if (item)
      return item->widget();
    return nullptr;
  }

  // QWidget interface
protected:
  virtual void paintEvent(QPaintEvent* event) override;

  // TODO 默认的拖拽、移动功能实现

private:
  QVBoxLayout _layout;
};

}

#pragma once

#include "Eyestack/Base.hpp"

namespace Eyestack::Design {

/**
 * @brief 滚动列表控件，包含一个自适应宽度、高度无限的 _board 容器，其宽度
 * 始终与 _board 保持一致、高度始终为建议尺寸（_board.sizeHint）。
 */
class EYESTACK_DESIGN_EXPORT ListScroll : public QScrollArea
{
  Q_OBJECT

  using _T = ListScroll;
  using _S = QScrollArea;

public:
  /**
   * @brief QScrollArea 的子控件，对其进行修改时应当心，例如：不要调用
   * setLayout 修改其布局，更不应该修改其 parent。
   */
  QWidget _board;

  /**
   * @brief _board 的布局对象，如果想向布局添加控件，使用 add_entry
   * 而不要直接调用此对象上的方法，前者可以自动调整 _board 的高度。
   */
  QVBoxLayout _boardLayout{ &_board };

public:
  ListScroll(QWidget* parent = nullptr)
    : _S(parent)
  {
    setWidget(&_board);
    setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    resizeBoard();
  }

public:
  /**
   * @brief 向布局中添加控件，同时更新 _board 尺寸。
   * @param widget 移交语义
   */
  void add_entry(QWidget* widget)
  {
    _boardLayout.addWidget(widget);

    // addWidget 是异步的，布局对象在添加控件后不会立即改变 sizeHint，所以使用
    // singleShot 异步更新界面。
    QTimer::singleShot(0, this, &_T::resizeBoard);
  }

  /**
   * @brief 向布局中添加子布局，同时更新 _board 尺寸。
   * @param layout 移交语义
   */
  void add_entry(QLayout* layout)
  {
    _boardLayout.addLayout(layout);
    QTimer::singleShot(0, this, &_T::resizeBoard);
  }

  /**
   * @brief 向布局中添加占位空间，同时更新 _board 尺寸。
   * @param spacing 占位空间高度
   */
  void add_entry(int spacing)
  {
    _boardLayout.addSpacing(spacing);
    QTimer::singleShot(0, this, &_T::resizeBoard);
  }

  /**
   * @brief 向布局中添加一项，同时更新 _board 尺寸。
   * @param item 移交语义
   */
  void add_entry(QLayoutItem* item)
  {
    _boardLayout.addItem(item);
    QTimer::singleShot(0, this, &_T::resizeBoard);
  }

public slots:
  /**
   * @brief 重新计算 _board 的尺寸并更新
   */
  void resizeBoard()
  {
    auto vp = viewport();
    //    if (vp == nullptr)
    //      return;
    _board.resize(vp->size().width(), _board.sizeHint().height());
  }

private:
  using _S::setWidget;
  using _S::widget;

  // QWidget interface
protected:
  virtual void resizeEvent(QResizeEvent* event) override;
  virtual QSize sizeHint() const override;
};

}

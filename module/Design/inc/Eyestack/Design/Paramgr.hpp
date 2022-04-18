#pragma once

namespace Eyestack::Design {

// ==========================================================================
// Paramgr
// ==========================================================================

/**
 * @brief 参数管理器UI组件类，基于 Qt Model/View 框架实现。这个组件的设计同时面
 * 向二次开发与最终使用界面的用户，提供参数读写、导入/出、重置的功能。。
 */
class EYESTACK_DESIGN_EXPORT Paramgr : public QWidget
{
  Q_OBJECT

  using _T = Paramgr;
  using _S = QWidget;

public:
  class Table;
  class Param;

public:
  Paramgr(QWidget* parent = nullptr);

public:
  /**
   * @brief 获取与控件关联的参数表对象的指针
   * @return 如果没有关联参数表，返回 nullptr
   */
  Table* get_table() { return (Table*)_treeView.model(); }

  /**
   * @brief 将参数表绑定到控件，当心指针悬挂
   * @param table 参数表对象
   */
  void set_table(Table& table);

private:
  class Delegate : public QStyledItemDelegate
  {
    // QAbstractItemDelegate interface
  public:
    virtual QWidget* createEditor(QWidget* parent,
                                  const QStyleOptionViewItem& option,
                                  const QModelIndex& index) const override;
    virtual void setEditorData(QWidget* editor,
                               const QModelIndex& index) const override;
    virtual void setModelData(QWidget* editor,
                              QAbstractItemModel* model,
                              const QModelIndex& index) const override;
    virtual void updateEditorGeometry(QWidget* editor,
                                      const QStyleOptionViewItem& option,
                                      const QModelIndex& index) const override;
  };

private:
  QPushButton _exportButton, _importButton, _resetButton;
  QTreeView _treeView;
  Delegate _delegate;

private slots:
  void when_resetButton_clicked();
  void when_exportButton_clicked();
  void when_importButton_clicked();
};

// ==========================================================================
// Paramgr::Table
// ==========================================================================

/**
 * @brief 参数表类，提供参数的注册管理功能
 */
class EYESTACK_DESIGN_EXPORT Paramgr::Table : private QStandardItemModel
{
  using _T = Table;
  using _S = QStandardItemModel;

public:
  using GroupPtr = void*;

public:
  Table();

public:
  /**
   * @brief 注册参数，如果已注册到其它参数表，则解除之前的注册。
   * @param param 参数对象
   */
  void register_param(Param& param);

  /**
   * @brief 注册参数到某一个分组下，如果已注册到其它参数表，则解除之前的注册。
   * @param param 参数对象
   * @param groupPtr 分组的指针，必须是有效的
   */
  void register_param(Param& param, GroupPtr groupPtr);

  /**
   * @brief 添加一个分组
   * @param name 分组名
   * @param desc 分组描述
   * @return 新分组的指针
   */
  GroupPtr add_group(QString name = QString(), QString desc = QString());

  /**
   * @brief 重置整个参数表为默认值。
   */
  void reset() noexcept;

  /**
   * @brief 序列化参数表。参数表的结构信息（参数个数、类型、组数）由程序先验定
   * 义，因而不会被序列化保存，也不会被反序列化保存。
   * @param ds 输出流
   * @return 成功返回 true，失败返回 false，若失败，则 ds 可能处于断裂状态。
   */
  bool dump(QDataStream& ds) noexcept(false);

  /**
   * @brief 反序列化参数表。参数表的结构必须与流中一致，否则可能反序列化失败。
   * @param ds 输入流
   * @return 成功返回 true，失败返回 false，若失败，则 ds 可能处于断裂状态。
   */
  bool load(QDataStream& ds) noexcept(false);

private:
  friend class Paramgr; // Paramgr 需要知道自己继承 QStandardItemModel
};

// ==========================================================================
// Paramgr::Param
// ==========================================================================

/**
 * @brief 参数类接口，它是不可复制、不可移动类型。
 */
class Paramgr::Param
{
  Param(const Param& other) = delete;
  Param(Param&& other) = delete;
  Param& operator=(const Param& other) = delete;
  Param& operator=(Param&& other) = delete;

public:
  /**
   * @param 参数名
   * @param 参数描述
   */
  Param(QString name, QString desc)
    : _name(std::move(name))
    , _desc(std::move(desc))
    , _nameItem(nullptr)
    , _descItem(nullptr)
    , _reprItem(nullptr)
  {}
  virtual ~Param() = default;

public:
  /**
   * @brief 注销，解除和参数表的绑定。如果参数没有绑定，则该方法不做任何事。
   */
  void unregister();

public:
  /**
   * @brief 获取参数名
   */
  const QString& get_name() { return _name; }

  /**
   * @brief 设置参数名
   */
  void set_name(QString name)
  {
    _name = std::move(name);
    if (_nameItem)
      _nameItem->setText(_name);
  }

  /**
   * @brief 获取参数描述
   */
  const QString& get_desc() { return _desc; }

  /**
   * @brief 设置参数描述
   */
  void set_desc(QString desc)
  {
    _desc = std::move(desc);
    if (_descItem)
      _descItem->setText(_desc);
  }

public:
  /**
   * @brief 获取参数值的字符串表达形式
   */
  virtual QString repr() = 0;

  /**
   * @brief 重置参数为默认值
   */
  virtual void reset() noexcept = 0;

  /**
   * @brief 序列化参数值
   * @param ds 输出流
   * @return 成功返回 true，失败返回 false，若失败，则 ds 可能处于断裂状态。
   */
  virtual bool dump(QDataStream& ds) noexcept(false) = 0;

  /**
   * @brief 反序列化参数值
   * @param ds 输入流
   * @return 成功返回 true，失败返回 false，若失败，则 ds 可能处于断裂状态。
   */
  virtual bool load(QDataStream& ds) noexcept(false) = 0;

protected:
  /**
   * @brief 更新的参数值表示，子类在参数值变化时，应当调用该函数刷新界面。
   */
  void update_repr()
  {
    if (_reprItem)
      _reprItem->setText(repr());
  }

  /**
   * @brief QStyledItemDelegate 回调，子类通过这些接口创建编辑器界面，详见 Qt
   * 文档。
   */
  virtual QWidget* create_editor(QWidget* parent,
                                 const QStyleOptionViewItem& option) = 0;

  /**
   * @brief QStyledItemDelegate 回调，子类通过这些接口创建编辑器界面，详见 Qt
   * 文档。
   */
  virtual void update_editor_geometry(QWidget* editor,
                                      const QStyleOptionViewItem& option) = 0;

  /**
   * @brief QStyledItemDelegate 回调，子类通过这些接口创建编辑器界面，详见 Qt
   * 文档。
   */
  virtual void set_editor_data(QWidget* editor) = 0;

  /**
   * @brief QStyledItemDelegate 回调，子类通过这些接口创建编辑器界面，详见 Qt
   * 文档。
   */
  virtual void set_model_data(QWidget* editor) = 0;

private:
  QString _name, _desc;
  QStandardItem *_nameItem, *_descItem, *_reprItem;

private:
  friend class Paramgr::Delegate; // Delegate 需要调用保护的纯虚接口
  friend class Table;             // Table 的注册需要看到成员变量
};

namespace paramgr {

// ==========================================================================
// paramgr::Bool
// ==========================================================================

class Bool : public Paramgr::Param
{
  using _S = Paramgr::Param;

public:
  /**
   * @brief 布尔类型参数类
   * @param name 参数名
   * @param desc 参数描述
   * @param val 参数默认值，也用作初始值
   */
  Bool(QString name, QString desc = "bool", bool val = false)
    : _S(std::move(name), std::move(desc))
    , _val(val)
    , _default(val)
  {
    update_repr();
  }

  operator bool&() { return _val; }

  Bool& operator=(bool val)
  {
    _val = val;
    update_repr();
    return *this;
  }

public:
  /**
   * @brief 获取内部变量的值
   * @return 内部变量的拷贝
   */
  bool val() { return _val; }

  /**
   * @brief 获取内部变量的引用
   * @return 内部变量的引用
   */
  bool& ref() { return _val; }

private:
  bool _val, _default;

  // Paramgr::Param interface
public:
  virtual QString repr() override { return _val ? "TRUE" : "FALSE"; }

  virtual void reset() noexcept override
  {
    _val = _default;
    update_repr();
  }

  virtual bool dump(QDataStream& ds) noexcept(false) override
  {
    ds << _val;
    return true;
  }

  virtual bool load(QDataStream& ds) noexcept(false) override
  {
    ds >> _val;
    update_repr();
    return true;
  }

protected:
  virtual QWidget* create_editor(QWidget* parent,
                                 const QStyleOptionViewItem& option) override;
  virtual void update_editor_geometry(
    QWidget* editor,
    const QStyleOptionViewItem& option) override;
  virtual void set_editor_data(QWidget* editor) override;
  virtual void set_model_data(QWidget* editor) override;
};

// ==========================================================================
// paramgr::Int
// ==========================================================================

class Int : public Paramgr::Param
{
  using _S = Paramgr::Param;

public:
  /**
   * @param name 参数名
   * @param desc 参数描述
   * @param val 参数默认值，也用作初始值
   * @param min 最小值约束，参数取值必须满足 min <= val
   * @param max 最大值约束，参数取值必须满足 val <= max
   */
  Int(QString name,
      QString desc = "int",
      int val = 0,
      int min = INT_MIN,
      int max = INT_MAX)
    : _S(std::move(name), std::move(desc))
    , _val(std::min(max, std::max(min, val)))
    , _default(val)
    , _min(min)
    , _max(max)
  {}

  operator int&() { return _val; }

  Int& operator=(int val)
  {
    _val = val;
    update_repr();
    return *this;
  }

public:
  /**
   * @brief 获取内部变量的值
   * @return 内部变量的拷贝
   */
  int val() { return _val; }

  /**
   * @brief 获取内部变量的引用
   * @return 内部变量的引用
   */
  int& ref() { return _val; }

private:
  int _val, _default;
  int _min, _max;

  // Paramgr::Param interface
public:
  virtual QString repr() override { return QString::number(_val); }

  virtual void reset() noexcept override
  {
    _val = _default;
    update_repr();
  }

  virtual bool dump(QDataStream& ds) noexcept(false) override
  {
    ds << _val;
    return true;
  }

  virtual bool load(QDataStream& ds) noexcept(false) override
  {
    ds >> _val;
    _val = std::min(_max, std::max(_min, _val));
    update_repr();
    return true;
  }

protected:
  virtual QWidget* create_editor(QWidget* parent,
                                 const QStyleOptionViewItem& option) override;
  virtual void update_editor_geometry(
    QWidget* editor,
    const QStyleOptionViewItem& option) override;
  virtual void set_editor_data(QWidget* editor) override;
  virtual void set_model_data(QWidget* editor) override;
};

// ==========================================================================
// paramgr::Float
// ==========================================================================

class Float : public Paramgr::Param
{
  using _S = Paramgr::Param;

public:
  /**
   * @param name 参数名
   * @param desc 参数描述
   * @param val 参数默认值，也用作初始值
   * @param min 最小值约束，参数取值必须满足 min <= val
   * @param max 最大值约束，参数取值必须满足 val <= max
   * @param decimals 小数位数
   */
  Float(QString name,
        QString desc = "float",
        float val = 0,
        float min = -FLT_MAX,
        float max = FLT_MAX,
        int decimals = 2)
    : _S(std::move(name), std::move(desc))
    , _val(std::min(max, std::max(min, val)))
    , _default(val)
    , _min(min)
    , _max(max)
    , _decimals(2)
  {}

  operator float&() { return _val; }

  Float& operator=(float val)
  {
    _val = val;
    update_repr();
    return *this;
  }

public:
  /**
   * @brief 获取内部变量的值
   * @return 内部变量的拷贝
   */
  float val() { return _val; }

  /**
   * @brief 获取内部变量的引用
   * @return 内部变量的引用
   */
  float& ref() { return _val; }

private:
  float _val, _default;
  float _min, _max;
  int _decimals;

  // Paramgr::Param interface
public:
  virtual QString repr() override { return QString::number(_val); }

  virtual void reset() noexcept override
  {
    _val = _default;
    update_repr();
  }

  virtual bool dump(QDataStream& ds) noexcept(false) override
  {
    ds << _val;
    return true;
  }

  virtual bool load(QDataStream& ds) noexcept(false) override
  {
    ds >> _val;
    _val = std::min(_max, std::max(_min, _val));
    update_repr();
    return true;
  }

protected:
  virtual QWidget* create_editor(QWidget* parent,
                                 const QStyleOptionViewItem& option) override;
  virtual void update_editor_geometry(
    QWidget* editor,
    const QStyleOptionViewItem& option) override;
  virtual void set_editor_data(QWidget* editor) override;
  virtual void set_model_data(QWidget* editor) override;
};

// ==========================================================================
// paramgr::Double
// ==========================================================================

class Double : public Paramgr::Param
{
  using _S = Paramgr::Param;

public:
  /**
   * @param name 参数名
   * @param desc 参数描述
   * @param val 参数默认值，也用作初始值
   * @param min 最小值约束，参数取值必须满足 min <= val
   * @param max 最大值约束，参数取值必须满足 val <= max
   */
  Double(QString name,
         QString desc = "double",
         double val = 0,
         double min = -DBL_MAX,
         double max = DBL_MAX,
         int decimals = 2)
    : _S(std::move(name), std::move(desc))
    , _val(std::min(max, std::max(min, val)))
    , _default(val)
    , _min(min)
    , _max(max)
    , _decimals(2)
  {}

  operator double&() { return _val; }

  Double& operator=(double val)
  {
    _val = val;
    update_repr();
    return *this;
  }

public:
  /**
   * @brief 获取内部变量的值
   * @return 内部变量的拷贝
   */
  double val() { return _val; }

  /**
   * @brief 获取内部变量的引用
   * @return 内部变量的引用
   */
  double& ref() { return _val; }

private:
  double _val, _default;
  double _min, _max;
  int _decimals;

  // Paramgr::Param interface
public:
  virtual QString repr() override { return QString::number(_val); }

  virtual void reset() noexcept override
  {
    _val = _default;
    update_repr();
  }

  virtual bool dump(QDataStream& ds) noexcept(false) override
  {
    ds << _val;
    return true;
  }

  virtual bool load(QDataStream& ds) noexcept(false) override
  {
    ds >> _val;
    _val = std::min(_max, std::max(_min, _val));
    update_repr();
    return true;
  }

protected:
  virtual QWidget* create_editor(QWidget* parent,
                                 const QStyleOptionViewItem& option) override;
  virtual void update_editor_geometry(
    QWidget* editor,
    const QStyleOptionViewItem& option) override;
  virtual void set_editor_data(QWidget* editor) override;
  virtual void set_model_data(QWidget* editor) override;
};

}

// ==========================================================================
// inline
// ==========================================================================

inline void
Paramgr::set_table(Table& table)
{
  setEnabled(true);
  _treeView.setModel(&table);
  _treeView.header()->resizeSections(QHeaderView::Stretch);
}

}

/** 程序员要如何使用 Paramgr
 *
 *  1. 参数类能像普通类型那样无缝使用：
 *      Double a;
 *      Bool b;
 *      if (b)
 *        a += 1;
 *
 *  2. 参数对象自己携带名字和说明信息：
 *      Double a(tr("name"), tr("a param used for ..."));
 *
 *  3. 通过一个注册操作，就可以在界面上读写这个参数：
 *      String s;
 *      paramgr.register(&s);
 *
 *  4. 用户在界面上的修改实时同步到参数对象，参数对象的修改也实时刷新到用户界
 *     面：
 *      Int32 a;
 *      paramgr.register(&a);
 *      a += 365;
 *
 *  5. 一个参数对象同时只能与一个管理器相绑定：
 *      Int32 a;
 *      paramgr1.register(&a);
 *      paramgr2.register(&a); // a 自动与 paramgr1 解绑
 *
 *  6. 参数对象与管理器对象的生命周期之间无相互约束，如果某一方析构，则自动解
 *     绑（观察者1对多模式）：
 *      Int32 a;
 *      {
 *        Paramgr paramgr;
 *        paramgr.register(&a);
 *      } // a 在这里自动与 paramgr 解绑
 *
 *  7. Paramgr 提供一些方便的工厂函数：
 *      auto a = paramgr.make_int32()
 */

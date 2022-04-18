#include "Eyestack/Design/Paramgr.hpp"
#include "Eyestack/Asset.hpp"
#include "Eyestack/Base.hpp"

namespace Eyestack::Design {

using namespace Base::util;

// ==========================================================================
// Paramgr
// ==========================================================================

Paramgr::Paramgr(QWidget* parent)
  : _S(parent)
  , _exportButton(tr("导出"))
  , _importButton(tr("导入"))
  , _resetButton(tr("重置"))
{
  // 未绑定参数表时整个组件都禁用掉
  setEnabled(false);

  // 设置样式
  _exportButton.setIcon(Asset::faicon("file-export-solid"));
  _importButton.setIcon(Asset::faicon("file-import-solid"));
  _resetButton.setIcon(Asset::faicon("undo-alt-solid"));
  _treeView.setItemDelegate(&_delegate);

  // 布局界面
  auto hLayout = new QHBoxLayout();
  hLayout->addWidget(&_exportButton);
  hLayout->addWidget(&_importButton);
  hLayout->addWidget(&_resetButton);

  auto vLayout = new QVBoxLayout();
  vLayout->addLayout(hLayout);
  vLayout->addWidget(&_treeView);

  setLayout(vLayout);

  // 连接信号
  connect(&_exportButton,
          &QPushButton::clicked,
          this,
          &_T::when_exportButton_clicked);
  connect(&_importButton,
          &QPushButton::clicked,
          this,
          &_T::when_importButton_clicked);
  connect(
    &_resetButton, &QPushButton::clicked, this, &_T::when_resetButton_clicked);
}

void
Paramgr::when_resetButton_clicked()
{
  auto table = get_table();
  if (!table)
    return;

  if (QMessageBox::question(
        this,
        tr("参数重置"),
        tr("确认重置所有参数为默认值？将丢失所有未保存的参数！"),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No) == QMessageBox::Yes) {
    table->reset();
  }
}

void
Paramgr::when_exportButton_clicked()
{
  auto table = get_table();
  if (!table)
    return;

  QString fileName = QFileDialog::getSaveFileName(
    this, tr("参数导出"), QString(), tr("参数文件 (*.params)"));
  if (fileName.isEmpty())
    return;

  QFile file(fileName);
  if (!file.open(QFile::WriteOnly)) {
    popmsg::open_file_failed(fileName);
    return;
  }

  QDataStream ds(&file);
  if (table->dump(ds)) {
    QMessageBox::information(
      this, tr("参数导出"), tr("参数成功导出到文件 \"%1\"").arg(fileName));
  } else {
    QMessageBox::critical(this, tr("导出参数"), tr("参数导出失败"));
  }
}

void
Paramgr::when_importButton_clicked()
{
  auto table = get_table();
  if (!table)
    return;

  QString fileName = QFileDialog::getOpenFileName(
    this, tr("参数导入"), QString(), tr("参数文件 (*.params)"));
  if (fileName.isEmpty())
    return;

  QFile file(fileName);
  if (!file.open(QFile::ReadOnly)) {
    popmsg::open_file_failed(fileName);
    return;
  }

  QDataStream ds(&file);
  if (table->load(ds)) {
    QMessageBox::information(
      this, tr("参数导入"), tr("成功从文件 \"%1\" 导入参数").arg(fileName));
  } else {
    table->reset();
    QMessageBox::critical(
      this, tr("参数导入"), tr("参数导入失败，参数表已重置为默认值。"));
  }
}

// ==========================================================================
// Paramgr::Delegate
// ==========================================================================

constexpr static const int kParamRole = Qt::UserRole + 1;

QWidget*
Paramgr::Delegate::createEditor(QWidget* parent,
                                const QStyleOptionViewItem& option,
                                const QModelIndex& index) const
{
  if (index.column() != 2)
    return nullptr;

  auto data = index.siblingAtColumn(0).data(kParamRole);
  if (data.isNull())
    return nullptr;

  auto ptr = reinterpret_cast<Param*>(data.value<void*>());
  if (ptr != nullptr)
    return ptr->create_editor(parent, option);

  return nullptr;
}

void
Paramgr::Delegate::setEditorData(QWidget* editor,
                                 const QModelIndex& index) const
{
  auto data = index.siblingAtColumn(0).data(kParamRole);
  Q_ASSERT(!data.isNull());

  auto ptr = reinterpret_cast<Param*>(data.value<void*>());
  Q_ASSERT(ptr != nullptr);

  ptr->set_editor_data(editor);
}

void
Paramgr::Delegate::setModelData(QWidget* editor,
                                QAbstractItemModel* model,
                                const QModelIndex& index) const
{
  auto data = index.siblingAtColumn(0).data(kParamRole);
  Q_ASSERT(!data.isNull());

  auto ptr = reinterpret_cast<Param*>(data.value<void*>());
  Q_ASSERT(ptr != nullptr);

  ptr->set_model_data(editor);
}

void
Paramgr::Delegate::updateEditorGeometry(QWidget* editor,
                                        const QStyleOptionViewItem& option,
                                        const QModelIndex& index) const
{
  auto data = index.siblingAtColumn(0).data(kParamRole);
  Q_ASSERT(!data.isNull());

  auto ptr = reinterpret_cast<Param*>(data.value<void*>());
  Q_ASSERT(ptr != nullptr);

  ptr->update_editor_geometry(editor, option);
}

// ==========================================================================
// Paramgr::Table
// ==========================================================================

Paramgr::Table::Table()
  : _S(0, 3)
{
  setHeaderData(0, Qt::Horizontal, tr("参数名"), Qt::DisplayRole);
  setHeaderData(1, Qt::Horizontal, tr("描述"), Qt::DisplayRole);
  setHeaderData(2, Qt::Horizontal, tr("取值"), Qt::DisplayRole);
}

void
Paramgr::Table::register_param(Param& param)
{
  param.unregister();

  param._nameItem = new QStandardItem(param.get_name());
  param._descItem = new QStandardItem(param.get_desc());
  param._reprItem = new QStandardItem(param.repr());

  appendRow({ param._nameItem, param._descItem, param._reprItem });

  param._nameItem->setData(QVariant::fromValue((void*)&param), kParamRole);
}

void
Paramgr::Table::register_param(Param& param, GroupPtr groupPtr)
{
  param.unregister();

  param._nameItem = new QStandardItem(param.get_name());
  param._descItem = new QStandardItem(param.get_desc());
  param._reprItem = new QStandardItem(param.repr());

  auto& group = *reinterpret_cast<QStandardItem*>(groupPtr);
  group.appendRow({ param._nameItem, param._descItem, param._reprItem });

  param._nameItem->setData(QVariant::fromValue((void*)&param), kParamRole);
}

Paramgr::Table::GroupPtr
Paramgr::Table::add_group(QString name, QString desc)
{
  auto nameItem = new QStandardItem(std::move(name));
  auto descItem = new QStandardItem(std::move(desc));

  appendRow({ nameItem, descItem });
  return nameItem;
}

void
Paramgr::Table::reset() noexcept
{
  auto root = invisibleRootItem();

  for (int i = 0; i < root->rowCount(); ++i) {
    auto item = root->child(i);
    if (item->rowCount() == 0) {
      auto& param = *reinterpret_cast<Param*>(item->data().value<void*>());
      param.reset();

    } else {
      for (int j = 0; j < item->rowCount(); ++j) {
        auto subitem = item->child(j);
        auto& param = *reinterpret_cast<Param*>(subitem->data().value<void*>());
        param.reset();
      }
    }
  }
}

bool
Paramgr::Table::dump(QDataStream& ds)
{
  auto root = invisibleRootItem();

  for (int i = 0; i < root->rowCount(); ++i) {
    auto item = root->child(i);
    if (item->rowCount() == 0) {
      auto& param = *reinterpret_cast<Param*>(item->data().value<void*>());
      if (!param.dump(ds))
        return false;

    } else {
      for (int j = 0; j < item->rowCount(); ++j) {
        auto subitem = item->child(j);
        auto& param = *reinterpret_cast<Param*>(subitem->data().value<void*>());
        if (!param.dump(ds))
          return false;
      }
    }
  }

  return true;
}

bool
Paramgr::Table::load(QDataStream& ds)
{
  auto root = invisibleRootItem();

  for (int i = 0; i < root->rowCount(); ++i) {
    auto item = root->child(i);
    if (item->rowCount() == 0) {
      auto& param = *reinterpret_cast<Param*>(item->data().value<void*>());
      if (!param.load(ds))
        return false;

    } else {
      for (int j = 0; j < item->rowCount(); ++j) {
        auto subitem = item->child(j);
        auto& param = *reinterpret_cast<Param*>(subitem->data().value<void*>());
        if (!param.load(ds))
          return false;
      }
    }
  }

  return true;
}

// ==========================================================================
// Paramgr::Param
// ==========================================================================

void
Paramgr::Param::unregister()
{
  if (_nameItem) {
    if (_nameItem->parent()) {
      _nameItem->parent()->removeRow(_nameItem->row());
    } else {
      auto model = reinterpret_cast<QStandardItemModel*>(_nameItem->model());
      model->removeRow(_nameItem->row());
    }
    _nameItem = _descItem = _reprItem = nullptr;
  }
}

namespace paramgr {

// ==========================================================================
// paramgr::Bool
// ==========================================================================

QWidget*
Bool::create_editor(QWidget* parent, const QStyleOptionViewItem& option)
{
  auto checkBox = new QCheckBox(parent);
  checkBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  checkBox->setAutoFillBackground(true);
  return checkBox;
}

void
Bool::update_editor_geometry(QWidget* editor,
                             const QStyleOptionViewItem& option)
{
  auto& checkBox = *reinterpret_cast<QCheckBox*>(editor);
  checkBox.setGeometry(option.rect);
}

void
Bool::set_editor_data(QWidget* editor)
{
  auto& checkBox = *reinterpret_cast<QCheckBox*>(editor);
  checkBox.setChecked(_val);
}

void
Bool::set_model_data(QWidget* editor)
{
  auto& checkBox = *reinterpret_cast<QCheckBox*>(editor);
  _val = checkBox.isChecked();
  update_repr();
}

// ==========================================================================
// paramgr::Int
// ==========================================================================

QWidget*
Int::create_editor(QWidget* parent, const QStyleOptionViewItem& option)
{
  auto spinBox = new QSpinBox(parent);
  spinBox->setMinimum(_min);
  spinBox->setMaximum(_max);
  return spinBox;
}

void
Int::update_editor_geometry(QWidget* editor, const QStyleOptionViewItem& option)
{
  auto& spinBox = *reinterpret_cast<QSpinBox*>(editor);
  spinBox.setGeometry(option.rect);
}

void
Int::set_editor_data(QWidget* editor)
{
  auto& spinBox = *reinterpret_cast<QSpinBox*>(editor);
  spinBox.setValue(_val);
}

void
Int::set_model_data(QWidget* editor)
{
  auto& spinBox = *reinterpret_cast<QSpinBox*>(editor);
  _val = spinBox.value();
  update_repr();
}

// ==========================================================================
// paramgr::Float
// ==========================================================================

QWidget*
Float::create_editor(QWidget* parent, const QStyleOptionViewItem& option)
{
  auto spinBox = new QDoubleSpinBox(parent);
  spinBox->setStepType(QDoubleSpinBox::AdaptiveDecimalStepType);
  spinBox->setDecimals(_decimals);
  spinBox->setMinimum(_min);
  spinBox->setMaximum(_max);
  return spinBox;
}

void
Float::update_editor_geometry(QWidget* editor,
                              const QStyleOptionViewItem& option)
{
  auto& spinBox = *reinterpret_cast<QDoubleSpinBox*>(editor);
  spinBox.setGeometry(option.rect);
}

void
Float::set_editor_data(QWidget* editor)
{
  auto& spinBox = *reinterpret_cast<QDoubleSpinBox*>(editor);
  spinBox.setValue(_val);
}

void
Float::set_model_data(QWidget* editor)
{
  auto& spinBox = *reinterpret_cast<QDoubleSpinBox*>(editor);
  _val = spinBox.value();
  update_repr();
}

// ==========================================================================
// paramgr::Double
// ==========================================================================

QWidget*
Double::create_editor(QWidget* parent, const QStyleOptionViewItem& option)
{
  auto spinBox = new QDoubleSpinBox(parent);
  spinBox->setStepType(QDoubleSpinBox::AdaptiveDecimalStepType);
  spinBox->setDecimals(_decimals);
  spinBox->setMinimum(_min);
  spinBox->setMaximum(_max);
  return spinBox;
}

void
Double::update_editor_geometry(QWidget* editor,
                               const QStyleOptionViewItem& option)
{
  auto& spinBox = *reinterpret_cast<QDoubleSpinBox*>(editor);
  spinBox.setGeometry(option.rect);
}

void
Double::set_editor_data(QWidget* editor)
{
  auto& spinBox = *reinterpret_cast<QDoubleSpinBox*>(editor);
  spinBox.setValue(_val);
}

void
Double::set_model_data(QWidget* editor)
{
  auto& spinBox = *reinterpret_cast<QDoubleSpinBox*>(editor);
  _val = spinBox.value();
  update_repr();
}

}

}

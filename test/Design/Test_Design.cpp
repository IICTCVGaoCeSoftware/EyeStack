#include "Test_Design.hpp"

#define BOOST_TEST_MODULE Test_Design
#include <boost/test/unit_test.hpp>

// ==========================================================================
// Paramgr Test
// ==========================================================================

#include "Eyestack/Design/Paramgr.hpp"

using namespace Eyestack::Design;

QWidget*
test_Paramgr()
{
  using namespace paramgr;

  auto table = new Paramgr::Table();

  auto bool1 = new Bool("bool1", "测试布尔类型", true);
  auto int1 = new Int("int1");
  auto float1 = new Float("float1", "测试 float 类型", 123);
  auto double1 = new Double("double1");

  table->register_param(*bool1);
  auto group = table->add_group("group1");
  table->register_param(*int1, group);
  table->register_param(*float1, group);
  table->register_param(*double1);

  auto paramgr = new Paramgr();
  paramgr->set_table(*table);

  return paramgr;
}

// ==========================================================================
// ImageLabel Test
// ==========================================================================

#include "Eyestack/Design/ImageLabel.hpp"

QWidget*
test_ImageLabel_for_image()
{
  auto imageLabel = new ImageLabel();

  QImage image(400, 300, QImage::Format_ARGB32);
  QPainter painter(&image);
  painter.setBrush(Qt::red);
  painter.drawRect(30, 30, 100, 100);
  painter.setBrush(Qt::blue);
  painter.drawEllipse(150, 150, 100, 100);

  imageLabel->display(image);

  return imageLabel;
}

QWidget*
test_ImageLabel_for_text()
{
  auto imageLabel = new ImageLabel();

  imageLabel->setFont(QFont("JetBrains Mono"));
  imageLabel->display("abc\nde\nf\n不开心");

  return imageLabel;
}

// ==========================================================================
// Monitor Test
// ==========================================================================

#include "Eyestack/Design/Monitor.hpp"

QWidget*
test_Monitor()
{
  auto monitor = new Eyestack::Design::Monitor();
  monitor->set_caption("这里是监视器标签");
  return monitor;
}

// ==========================================================================
// QuadrangleSelector Test
// ==========================================================================

#include "Eyestack/Design/QuadrangleSelector.hpp"

QWidget*
test_QuadrangleSelector()
{
  QImage image(32, 32, QImage::Format_ARGB32);
  QPainter painter(&image);
  painter.setPen(Qt::red);
  painter.setBrush(Qt::yellow);
  painter.drawEllipse(1, 1, 30, 30);

  auto widget = new QuadrangleSelector(false);
  widget->display(image);
  widget->reset_thumbtacks();

  return widget;
}

// ==========================================================================
// RectangleSelector Test
// ==========================================================================

#include "Eyestack/Design/RectangleSelector.hpp"

QWidget*
test_RectangleSelector()
{
  QImage image(128, 128, QImage::Format_ARGB32);
  QPainter painter(&image);
  painter.setPen(Qt::red);
  painter.setBrush(Qt::yellow);
  painter.drawEllipse(14, 14, 100, 100);

  auto widget = new RectangleSelector();
  widget->display(image);
  widget->reset_selection();

  return widget;
}

// ==========================================================================
// StyleWindow Test
// ==========================================================================

#include "Eyestack/Design/StyleWindow.hpp"

QWidget*
test_StyleWindow()
{
  auto window = new StyleWindow();
  window->resize(400, 300);
  window->setWindowFlag(Qt::WindowStaysOnTopHint);

  window->set_body(new QTextEdit());

  auto button = new QPushButton("显示窗口");

  QObject::connect(button, &QPushButton::clicked, window, &QWidget::show);
  QObject::connect(
    button, &QPushButton::destroyed, window, &QObject::deleteLater);

  return button;
}

// ==========================================================================
// test_Design
// ==========================================================================

#include "Eyestack/Asset.hpp"

BOOST_AUTO_TEST_CASE(test_Design)
{
  static int argc = 0;
  QApplication app(argc, nullptr);

  app.setStyle("fusion");

  Eyestack::Asset::ginit();

  QMessageBox testctrl(QMessageBox::Question,
                       "测试控制",
                       "测试通过？",
                       QMessageBox::Yes | QMessageBox::No);
  testctrl.setModal(false);
  testctrl.show();

  app.connect(&testctrl, &QMessageBox::accepted, &app, &QApplication::quit);
  app.connect(&testctrl, &QMessageBox::rejected, [&app]() { app.exit(-1); });

  MainWindow mainWindow;
  mainWindow.show();

  mainWindow.add_test("ImageLabel 显示图像的效果", test_ImageLabel_for_image);
  mainWindow.add_test("ImageLabel 显示文本的效果", test_ImageLabel_for_text);
  mainWindow.add_test("Monitor 组件测试", test_Monitor);
  mainWindow.add_test("参数管理器测试", test_Paramgr);
  mainWindow.add_test("四边形选择器测试", test_QuadrangleSelector);
  mainWindow.add_test("矩形选择器测试", test_RectangleSelector);
  mainWindow.add_test("StyleWindow 测试", test_StyleWindow);

  BOOST_TEST(app.exec() == 0);
}

MainWindow::MainWindow()
{
  setCentralWidget(&_leftList);
  connect(&_leftList,
          &QListWidget::itemDoubleClicked,
          this,
          &_T::when_leftList_itemDoubleClicked);
}

void
MainWindow::add_test(const QString& name, TestFunc test)
{
  auto item = new QListWidgetItem(name);
  item->setData(Qt::UserRole,
                QVariant::fromValue(reinterpret_cast<void*>(test)));
  _leftList.addItem(item);
}

void
MainWindow::when_leftList_itemDoubleClicked(QListWidgetItem* item)
{
  auto dialog = new QDialog(this);
  dialog->setWindowTitle(item->text());

  auto layout = new QHBoxLayout(dialog);
  auto widget = TestFunc(item->data(Qt::UserRole).value<void*>())();
  layout->addWidget(widget);

  dialog->setAttribute(Qt::WA_DeleteOnClose);
  dialog->setWindowModality(Qt::WindowModal);
  dialog->show();
}

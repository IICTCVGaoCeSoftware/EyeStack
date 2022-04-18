#pragma once

class MainWindow : public QMainWindow
{
  Q_OBJECT

  using _T = MainWindow;
  using _S = QMainWindow;

public:
  using TestFunc = QWidget* (*)();

public:
  MainWindow();

public:
  /**
   * @brief 添加测试
   * @param name 测试名
   * @param test 测试函数
   */
  void add_test(const QString& name, TestFunc test);

private:
  QListWidget _leftList;

private slots:
  void when_leftList_itemDoubleClicked(QListWidgetItem* item);
};

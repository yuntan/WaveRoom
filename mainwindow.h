#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class WaveCalculator;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

 private slots:
  void on_actionQuit_triggered();

 private:
  Ui::MainWindow *ui;
  WaveCalculator *calc;
};

#endif  // MAINWINDOW_H

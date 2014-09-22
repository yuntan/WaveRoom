#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QStyleFactory>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  // make app looks like native kde app
  // http://stackoverflow.com/questions/14880032/native-kde-look-for-qt-5-app
  QApplication::setStyle(QStyleFactory::create("GTK+"));

  ui->setupUi(this);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_actionQuit_triggered()
{
    QApplication::quit();
}

#include <QApplication>
#include <QStyleFactory>

#include "iostream"
#include "wavecalculator.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  // make app looks like native kde app
  // http://stackoverflow.com/questions/14880032/native-kde-look-for-qt-5-app
  QApplication::setStyle(QStyleFactory::create("GTK+"));

  calc = new WaveCalculator(this);
  calc->start();

  ui->setupUi(this);
  ui->glWidget->faces = calc->faces;

  connect(calc, SIGNAL(facesUpdated()), ui->glWidget, SLOT(updateGL()));
//  connect(calc, &WaveCalculator::facesUpdated,
//          []() { std::printf("faces updated!"); });
}

MainWindow::~MainWindow() {
  calc->stop();
  delete ui;
}

void MainWindow::on_actionQuit_triggered() { QApplication::quit(); }

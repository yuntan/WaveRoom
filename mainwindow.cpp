#include <QApplication>
#include <QStyleFactory>
#include <QList>
#include <QTimer>
#include <QScriptEngine>
#include <QMetaEnum>
#include <QProgressDialog>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "wavecalculator.h"
#include "facedata.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  faces = new QList<FaceData *>();

  ui->setupUi(this);

  // setup glWidget
  ui->glWidget->face = new FaceData(0, this);

  // setup boundCombo
  auto calcMetaObj = WaveCalculator::staticMetaObject;
  auto bc = calcMetaObj.enumerator(calcMetaObj.indexOfEnumerator("BC"));
  for (int i = 0; i < bc.keyCount(); i++) {
    ui->boundCombo->addItem(bc.key(i), bc.value(i));
  }

  // setup progress dialog
  _dialog = new QProgressDialog("Calculating...", "(can't cancel)", 0, 0, this);

  // setup timer
  timer = new QTimer(this);
  timer->setInterval(INTERVAL);

  connect(timer, &QTimer::timeout, [=]() {
    auto slider = ui->timeSlider;
    if (slider->value() == slider->maximum()) {
      timer->stop();
    } else {
      slider->setValue(slider->value() + 1);
      updateGL();
    }
  });
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::updateGL() {
  ui->glWidget->face = faces->at(ui->timeSlider->value());
  ui->glWidget->updateGL();
}

void MainWindow::on_calcButton_clicked() {
  //  QScriptEngine engine;
  auto engine = new QScriptEngine(this);  // have to use new
  engine->evaluate(ui->iniDistEdit->document()->toPlainText());
  engine->evaluate(ui->iniVEdit->document()->toPlainText());
  auto ini_dist = engine->globalObject().property("u0");
  auto ini_velo = engine->globalObject().property("v0");

  int step = ui->stepEdit->value();
  dt = (float)ui->dtEdit->value();
  float wave_v = (float)ui->waveVEdit->value();
  auto bc = (WaveCalculator::BC)ui->boundCombo->currentData().toInt();

  if (calc != NULL) {
    delete calc;
  }
  calc = new WaveCalculator(ini_dist, ini_velo, step, dt, wave_v, bc, this);

  connect(calc, &WaveCalculator::calcProgressChanged,
          [=](int max, int value) {
    _dialog->setMaximum(max);
    _dialog->setValue(value);
  });

  connect(calc, &WaveCalculator::calcFinished, [=]() {
    timer->stop();
    if (faces != NULL) {
      qDeleteAll(*faces);
      faces->clear();
      delete faces;
    }
    faces = calc->result();
    ui->timeSlider->setMaximum(faces->length() - 1);
    updateGL();
  });

  connect(calc, &WaveCalculator::calcFinished, _dialog,
          &QProgressDialog::close);

  calc->startCalc();

  _dialog->open();
}

void MainWindow::on_playButton_clicked() {
  if (!timer->isActive()) {
    updateGL();
    timer->start();
  }
}

void MainWindow::on_pauseButton_clicked() { timer->stop(); }

void MainWindow::on_stopButton_clicked() {
  timer->stop();
  ui->timeSlider->setValue(0);
}

void MainWindow::on_timeSlider_valueChanged(int value) {
  ui->timeLabel->setText(QString("%1").arg(faces->at(value)->t(), 3, 'f', 2));
  updateGL();
}

void MainWindow::on_useVNChkBox_toggled(bool checked) {
  ui->glWidget->useVertexNormal = checked;
}

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QProgressDialog;
class QTimer;

class WaveCalculator;
class FaceData;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

 private slots:
  void on_calcButton_clicked();
  void on_playButton_clicked();
  void on_pauseButton_clicked();
  void on_timeSlider_valueChanged(int value);

  void on_useVNChkBox_toggled(bool checked);

  void on_stopButton_clicked();

 private:
  void updateGL();

  Ui::MainWindow *ui;
  QProgressDialog *_dialog;

  WaveCalculator *calc = NULL;
  QList<FaceData *> *faces = NULL;
  QTimer *timer;
  float dt;
  const int INTERVAL = 50;  // 50ms
};

#endif  // MAINWINDOW_H

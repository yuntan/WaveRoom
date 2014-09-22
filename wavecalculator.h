#ifndef WAVECALCULATOR_H
#define WAVECALCULATOR_H

#include <memory>
#include <QObject>

class QTimer;
class FaceData;

class WaveCalculator : public QObject {
  Q_OBJECT
 public:
  explicit WaveCalculator(QObject *parent = 0);
  void start();
  void stop();
  void reset();

  std::shared_ptr<QList<FaceData *>> faces;

signals:
  void facesUpdated();

 private:
  void initializeWaveSurface();
  void calc();

  QTimer *timer;
  const int INTERVAL = 16;  // 16ms
  int step = 0;
};

#endif  // WAVECALCULATOR_H

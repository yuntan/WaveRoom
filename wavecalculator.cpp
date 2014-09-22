#include <QTimer>
#include <QVector3D>

#include "facedata.h"
#include "consts.h"

#include "wavecalculator.h"

WaveCalculator::WaveCalculator(QObject *parent) : QObject(parent) {
  initializeWaveSurface();

  timer = new QTimer(this);
  timer->setInterval(INTERVAL);

  connect(timer, &QTimer::timeout, this, &WaveCalculator::calc);
}

void WaveCalculator::start() { timer->start(); }

void WaveCalculator::stop() { timer->stop(); }

void WaveCalculator::reset() {
  timer->stop();
  initializeWaveSurface();
}

void WaveCalculator::initializeWaveSurface() {
  float w = 100.0f / N;

  // build wave surface
  faces = std::make_shared<QList<FaceData *>>();
  for (int i = 0; i < N * N; i++) {
    faces->append(new FaceData(
        QVector3D(w * (i % N) - 50.0f, 0.0f, -w * (i / N) + 50.0f), w));
  }
}

void WaveCalculator::calc() {
  float R = 20, omega = M_PI / 50.0;

  // gaussian distribusion
  float meanX = R * cos(omega * step), meanZ = R * sin(omega * step),
        sigma2 = 200, maxY = 60;
  for (int i = 0; i < N * N; i++) {
    auto face = faces->at(i);
    auto vec = face->vertices;
    for (int j = 0; j < 4; j++) {
      float x = vec[j]->x(), z = vec[j]->z();
      vec[j]->setY((float)(maxY * exp(-((x - meanX) * (x - meanX) +
                                        (z - meanZ) * (z - meanZ)) /
                                      (2.0f * sigma2))));
    }
  }

  // emit signal
  facesUpdated();

  step++;
}

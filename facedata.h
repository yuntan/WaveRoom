#ifndef FACEDATA_H
#define FACEDATA_H

#include <QObject>
#include <QVector3D>

#include "consts.h"

class FaceData : public QObject {
  Q_OBJECT

 public:
  explicit FaceData(float t, QObject* parent = 0);
  float t() const { return _t; }
  QVector3D vertices[(N + 1) * (N + 1)];
  QVector3D vertexNormals[(N + 1) * (N + 1)];
  QVector3D rectNormals[N * N];

 private:
  float _t;
};

#endif  // FACEDATA_H

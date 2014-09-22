#ifndef FACEDATA_H
#define FACEDATA_H

#include <QObject>
#include <QRectF>

class FaceData : public QObject {
  Q_OBJECT

 public:
  explicit FaceData(const QVector3D&, float, QObject* parent = 0);
  QVector3D* vertices[4];
  QVector3D normal();

 private:
};

#endif  // FACEDATA_H

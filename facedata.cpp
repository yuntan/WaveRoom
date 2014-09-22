#include "facedata.h"
#include <QVector3D>

FaceData::FaceData(const QVector3D& lowerleft, float width, QObject* parent)
    : QObject(parent) {
  // set vertices
  for (int i = 0; i < 4; i++) {
    auto v = new QVector3D(lowerleft);
    if (i == 1 || i == 2) {
      v->setX(v->x() + width);
    }
    if (i == 2 || i == 3) {
      v->setZ(v->z() - width);
    }
    vertices[i] = v;
  }
}

QVector3D FaceData::normal() {
  return QVector3D::normal(*(vertices[0]), *(vertices[1]), *(vertices[3]));
}

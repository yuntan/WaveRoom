#include "consts.h"
#include "facedata.h"

FaceData::FaceData(float t, QObject* parent) : QObject(parent) {
  _t = t;

  // set vertices
  for (int i = 0; i < (N + 1) * (N + 1); i++) {
    vertices[i].setX(-SURFACE_SIDE / 2.0f + (i % (N + 1)) * RECT_SIDE);
    vertices[i].setZ(+SURFACE_SIDE / 2.0f - (i / (N + 1)) * RECT_SIDE);
    vertexNormals[i].setY(1.0f);
  }
  for (int i = 0; i < N * N; i++) {
    rectNormals[i].setY(1.0f);
  }
}

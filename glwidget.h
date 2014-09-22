#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QWidget>
#include "facedata.h"

class GLWidget : public QGLWidget {
  Q_OBJECT

 public:
  GLWidget(QWidget *parent = 0);
  bool useVertexNormal = true;

 private:
  void initializeGL();
  void resizeGL(int w, int h);
  void paintGL();

  void setupLight();
  void paintObject();
  void setupModelView();
  void setupProjection();

  QVector3D calcVertexNormal(int,int);

  // N x N faces on wave surface
  const int N = 100;

  QList<FaceData *> *_faces;
};

#endif  // GLWIDGET_H

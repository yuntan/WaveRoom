#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <memory>
#include <QGLWidget>

class FaceData;

class GLWidget : public QGLWidget {
  Q_OBJECT

 public:
  GLWidget(QWidget *parent = 0);

  bool useVertexNormal = true;
  FaceData *face;

 private:
  void initializeGL();
  void resizeGL(int w, int h);
  void paintGL();

  void setupLight();
  void paintObject();
  void setupModelView();
  void setupProjection();
};

#endif  // GLWIDGET_H

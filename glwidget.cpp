#include <GL/glu.h>
#include <QVector3D>
#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent) {
  float w = 100.0f / N;

  // build wave surface
  _faces = new QList<FaceData *>();
  for (int i = 0; i < N * N; i++) {
    _faces->append(new FaceData(
        QVector3D(w * (i % N) - 50.0f, 0.0f, -w * (i / N) + 50.0f), w));
  }

  // gaussian distribusion
  float meanX = 0, meanZ = 0, sigma2 = 200, maxY = 60;
  for (int i = 0; i < N * N; i++) {
    auto face = _faces->at(i);
    auto vec = face->vertices;
    for (int j = 0; j < 4; j++) {
      float x = vec[j]->x(), z = vec[j]->z();
      vec[j]->setY((float)(maxY * exp(-((x - meanX) * (x - meanX) +
                                        (z - meanZ) * (z - meanZ)) /
                                      (2.0f * sigma2))));
    }
  }
}

void GLWidget::initializeGL() {
  // background color
  qglClearColor(Qt::darkGray);

  // enable depth buffer
  glEnable(GL_DEPTH_TEST);
}

void GLWidget::resizeGL(int w, int h) {
  // setup viewport
  glViewport(0, 0, w, h);
}

void GLWidget::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glShadeModel(GL_SMOOTH);

  glViewport(0, 0, width(), height());

  paintObject();
  setupModelView();
  setupProjection();
  setupLight();

  glFlush();
}

void GLWidget::setupLight() {
  glEnable(GL_LIGHTING);

  const GLfloat lightPosition[4] = {30.0f, 100.0f, 30.0f, 1.0f};
  const GLfloat lightDiffuse[3] = {1.0f, 1.0f, 1.0f};   // white
  const GLfloat lightSpecular[3] = {1.0f, 1.0f, 1.0f};  // white
  const GLfloat lightAmbient[3] = {0.25f, 0.25f, 0.25f};

  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
  glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
}

void GLWidget::paintObject() {
  static GLfloat diffuse[3] = {0.01960f, 0.40000f, 0.72157f};  // #0566b8
  static GLfloat ambient[3] = {0.01960f, 0.40000f, 0.72157f};  // #0566b8
  static GLfloat specular[3] = {1.0f, 1.0f, 1.0f};
  static GLfloat shininess[1] = {125.0f};

  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

  for (int i = 0; i < N * N; i++) {
    auto face = _faces->at(i);
    auto v = face->vertices;

    glBegin(GL_POLYGON);
    {
      if (!useVertexNormal) {
        auto n = face->normal();
        glNormal3f(n.x(), n.y(), n.z());
      }
      for (int j = 0; j < 4; j++) {
        if (useVertexNormal) {
          auto n = calcVertexNormal(i, j);
          glNormal3f(n.x(), n.y(), n.z());
        }
        glVertex3f(v[j]->x(), v[j]->y(), v[j]->z());
      }
    }
    glEnd();
  }
}

void GLWidget::setupModelView() {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // camera: (40, 120, 40), look: (0, 0, 0), up: (0, 1, 0)
  gluLookAt(40.0, 100.0, 40.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void GLWidget::setupProjection() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, (double)width() / (double)height(), 1.0, 1000.0);
}

QVector3D GLWidget::calcVertexNormal(int i, int j) {
  if (i / N == 0 || i % N == 0 || i / N == N - 1 || i % N == N - 1) {
    return _faces->at(i)->normal();
  }

  QVector3D n = _faces->at(i)->normal();

  // faces
  /* | i+N-1 | i+N | i+N+1 |
   * |  i-1  |  i  |  i+1  |
   * | i-N-1 | i-N | i-N+1 |
   */
  switch (j) {
    case 0:
      n += _faces->at(i - N - 1)->normal();
      n += _faces->at(i - N)->normal();
      n += _faces->at(i - 1)->normal();
      break;
    case 1:
      n += _faces->at(i - N)->normal();
      n += _faces->at(i - N + 1)->normal();
      n += _faces->at(i + 1)->normal();
      break;
    case 2:
      n += _faces->at(i + 1)->normal();
      n += _faces->at(i + N)->normal();
      n += _faces->at(i + N + 1)->normal();
      break;
    case 3:
      n += _faces->at(i - 1)->normal();
      n += _faces->at(i + N - 1)->normal();
      n += _faces->at(i + N)->normal();
      break;
  }

  return n.normalized();
}

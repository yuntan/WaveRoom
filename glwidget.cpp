#include <GL/glu.h>
#include <QVector3D>
#include "glwidget.h"
#include "facedata.h"
#include "consts.h"

GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent) {}

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

//  const GLfloat lightPosition[4] = {30.0f, 100.0f, 30.0f, 1.0f};
  const GLfloat lightPosition[4] = {0.0f, 80.0f, 0.0f, 1.0f};
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
    glBegin(GL_POLYGON);
    {
      if (!useVertexNormal) {
        auto n = face->rectNormals[i];
        glNormal3f(n.x(), n.y(), n.z());
      }

      int lowerLeft = (i % N) + (i / N) * (N + 1);
      for (int j :
           {lowerLeft, lowerLeft + 1, lowerLeft + N + 2, lowerLeft + N + 1}) {
        if (useVertexNormal) {
          auto n = face->vertexNormals[j];
          glNormal3f(n.x(), n.y(), n.z());
        }
        auto v = face->vertices[j];
        glVertex3f(v.x(), v.y(), v.z());
      }
    }
    glEnd();
  }
}

void GLWidget::setupModelView() {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // camera: (40, 100, 40), look: (0, 0, 0), up: (0, 1, 0)
//  gluLookAt(40.0, 100.0, 40.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

  gluLookAt(0.0, 80.0, 80.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void GLWidget::setupProjection() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, (double)width() / (double)height(), 1.0, 1000.0);
}


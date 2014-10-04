#include <QVector3D>
#include <QtConcurrent>
#include <QScriptEngine>
#include <QScriptValue>
#include <QScriptValueList>
#include <QThread>
#include <QDebug>

#include "facedata.h"
#include "wavecalculator.h"

WaveCalculator::WaveCalculator(QScriptValue ini_dist, QScriptValue ini_velo,
                               int step, float dt, float wave_v, BC bc,
                               QObject* parent)
    : _ini_dist(ini_dist),
      _ini_velo(ini_velo),
      _step(step),
      _dt(dt),
      _wave_v(wave_v),
      _bc(bc),
      QObject(parent) {
  _watcher = new QFutureWatcher<QList<FaceData*>*>();

  connect(_watcher, &QFutureWatcher<QList<FaceData*>*>::finished, this,
          &WaveCalculator::calcFinished);
}

void WaveCalculator::startCalc() {
  auto calcFunc = [=]() -> QList<FaceData*> * {
    qDebug() << "Calculation started on thread " << QThread::currentThread();

    auto faces = new QList<FaceData*>();

    auto face = new FaceData(0);
    for (int i = 0; i < (N + 1) * (N + 1); i++) {
      auto v = face->vertices[i];
      QScriptValueList args;
      args << v.x() << v.z();
      v.setY(_ini_dist.call(QScriptValue(), args).toNumber());
      face->vertices[i] = v;
    }
    calcRectNormals(face);
    calcVertexNormals(face);
    faces->append(face);
    emit calcProgressChanged(_step, 1);

    face = new FaceData(_dt);
    for (int i = 0; i < (N + 1) * (N + 1); i++) {
      auto v = face->vertices[i];
      QScriptValueList args;
      args << v.x() << v.z();
      v.setY(_ini_dist.call(QScriptValue(), args).toNumber());
      face->vertices[i] = v;
    }
    calcRectNormals(face);
    calcVertexNormals(face);
    faces->append(face);
    emit calcProgressChanged(_step, 2);

    for (int i = 2; i < _step; i++) {
      // i is current surface's index
      faces->append(calcNextSurface(faces->at(i - 2), faces->at(i - 1), _dt,
                                    _wave_v, _bc));
      emit calcProgressChanged(_step, i + 1);
    }

    qDebug() << "Calculation finished";
    return faces;
  };

  _watcher->setFuture(QtConcurrent::run(calcFunc));
}

FaceData* WaveCalculator::calcNextSurface(const FaceData* previous,
                                          const FaceData* current, float dt,
                                          float wave_v, BC bc) {
  auto cv = current->vertices;
  auto pv = previous->vertices;
  float dx2 = RECT_SIDE * RECT_SIDE;  //Δx^2

  auto nextu = [=](int i, int up, int down, int left, int right) -> float {
    return 2 * cv[i].y() - pv[i].y() +
           dt * dt * wave_v * wave_v *
               ((cv[right].y() + cv[left].y() - 2 * cv[i].y()) / dx2 +
                (cv[up].y() + cv[down].y() - 2 * cv[i].y()) / dx2);
  };

  auto next = new FaceData(current->t() + dt);
  auto nv = next->vertices;
  for (int r = 1; r <= N - 1; r++) {    // row index
    for (int c = 1; c <= N - 1; c++) {  // column index
      int i = c + r * (N + 1);          // i is vertex index without boundary
      int up = c + (r + 1) * (N + 1), down = c + (r - 1) * (N + 1),
          left = (c - 1) + r * (N + 1), right = (c + 1) + r * (N + 1);

      nv[i].setY(nextu(i, up, down, left, right));
    }
  }

  // calculate vertices on boundary
  switch (bc) {
    case BC::Periodic: {
      for (int r = 1; r <= N - 1; r++) {  // row index
        // bottom side
        int i = 0 + r * (N + 1);
        int up = 0 + (r + 1) * (N + 1), down = 0 + (r - 1) * (N + 1),
            left = N + r * (N + 1), right = 1 + r * (N + 1);
        nv[i].setY(nextu(i, up, down, left, right));

        // top side
        i = N + r * (N + 1);
        up = N + (r + 1) * (N + 1), down = N + (r - 1) * (N + 1),
        left = (N - 1) + r * (N + 1), right = 0 + r * (N + 1);
        nv[i].setY(nextu(i, up, down, left, right));
      }

      for (int c = 1; c <= N - 1; c++) {  // column index
        // left side
        int i = c + 0 * (N + 1);  // i is vertex index without boundary
        int up = c + 1 * (N + 1), down = c + N * (N + 1),
            left = (c - 1) + 0 * (N + 1), right = (c + 1) + 0 * (N + 1);
        nv[i].setY(nextu(i, up, down, left, right));

        // right side
        i = c + N * (N + 1);  // i is vertex index without boundary
        up = c + 0 * (N + 1), down = c + (N - 1) * (N + 1),
        left = (c - 1) + N * (N + 1), right = (c + 1) + N * (N + 1);
        nv[i].setY(nextu(i, up, down, left, right));
      }

      // lower left
      int i = 0 + 0 * (N + 1);
      int up = 0 + 1 * (N + 1), down = 0 + N * (N + 1), left = N + 0 * (N + 1),
          right = 1 + 0 * (N + 1);
      nv[i].setY(nextu(i, up, down, left, right));

      // lower right
      i = N + 0 * (N + 1);
      up = N + 1 * (N + 1), down = N + N * (N + 1),
      left = (N - 1) + 0 * (N + 1), right = 0 + 0 * (N + 1);
      nv[i].setY(nextu(i, up, down, left, right));

      // upper left
      i = 0 + N * (N + 1);
      up = 0 + 0 * (N + 1), down = 0 + (N - 1) * (N + 1),
      left = N + N * (N + 1), right = 1 + N * (N + 1);
      nv[i].setY(nextu(i, up, down, left, right));

      // upper right
      i = N + N * (N + 1);
      up = N + 0 * (N + 1), down = N + (N - 1) * (N + 1),
      left = (N - 1) + N * (N + 1), right = 0 + N * (N + 1);
      nv[i].setY(nextu(i, up, down, left, right));
      break;
    }

    case BC::Dirichlet: {
      // left and right side
      for (int r = 0; r < N + 1; r++) {
        for (int c : {0, N}) {
          int i = c + r * (N + 1);  // i is vertec indec of boundarr
          nv[i].setY(0);
        }
      }

      // top and bottom side
      for (int r : {0, N}) {
        for (int c = 0; c < N + 1; c++) {
          int i = c + r * (N + 1);  // i is vertec indec of boundarr
          nv[i].setY(0);
        }
      }
      break;
    }

    case BC::Neumann: {
      // left and right side
      for (int r = 0; r <= N; r++) {  // r is row index
        nv[0 + r * (N + 1)].setY(nv[1 + r * (N + 1)].y());
        nv[N + r * (N + 1)].setY(nv[(N - 1) + r * (N + 1)].y());
      }

      // top and bottom side
      for (int c = 0; c <= N; c++) {
        nv[c + 0 * (N + 1)].setY(nv[c + 1 * (N + 1)].y());
        nv[c + N * (N + 1)].setY(nv[c + (N - 1) * (N + 1)].y());
      }
      break;
    }
  }

  calcRectNormals(next);
  calcVertexNormals(next);

  return next;
}

void WaveCalculator::calcRectNormals(FaceData* face) {
  for (int i = 0; i < N * N; i++) {   // i is rect index
    int j = i % N + i / N * (N + 1);  // j is vertex index
    face->rectNormals[i] = QVector3D::normal(
        face->vertices[j], face->vertices[j + 1], face->vertices[j + N + 1]);
  }
}

void WaveCalculator::calcVertexNormals(FaceData* face) {
  for (int i = 0; i < (N + 1) * (N * 1); i++) {       // i is vertex index
    QVector3D n;                                      // vertex normal
    int j = i % (N + 1) + (i / (N + 1) - 1) * N - 1;  // j is rect index
    for (int k : {j, j + 1, j + N, j + N + 1}) {
      if (k >= 0 && k <= N * N - 1) {
        n += face->rectNormals[k];
      }
    }

    face->vertexNormals[i] = n.normalized();
  }
}

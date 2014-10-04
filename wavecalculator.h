#ifndef WAVECALCULATOR_H
#define WAVECALCULATOR_H

#include <QObject>
#include <QScriptValue>
#include <QFutureWatcher>

#include "consts.h"

class FaceData;

class WaveCalculator : public QObject {
  Q_OBJECT
  Q_ENUMS(BC)

 public:
  // boundary conditions
  enum BC {
    Periodic,   // 周期端条件
    Dirichlet,  // 固定端条件
    Neumann,    // 自由端条件
  };

  explicit WaveCalculator(QScriptValue ini_dist, QScriptValue ini_velo,
                          int step, float dt, float wave_v, BC bc,
                          QObject* parent = 0);
  void startCalc();
  QList<FaceData*>* result() { return _watcher->result(); }

  QFutureWatcher<QList<FaceData*>*>* watcher() const { return _watcher; }

signals:
  void calcProgressChanged(int max, int value);
  void calcFinished();

 private:
  static FaceData* calcNextSurface(const FaceData* previous,
                                   const FaceData* current, float dt,
                                   float wave_v, BC bc);
  static void calcRectNormals(FaceData* face);
  static void calcVertexNormals(FaceData* face);

  QFutureWatcher<QList<FaceData*>*>* _watcher;

  // variants for calculation
  QScriptValue _ini_dist;
  QScriptValue _ini_velo;
  int _step;
  float _dt;
  float _wave_v;
  BC _bc;
};

#endif  // WAVECALCULATOR_H

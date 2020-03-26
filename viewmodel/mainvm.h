#ifndef MAINVM_H
#define MAINVM_H

#include <QSharedPointer>
#include <QElapsedTimer>
#include <QSize>

#include "model/simulator.h"
#include "model/simulatorthread.h"
#include "model/gradientsurface.h"
#include "util/simplevaluestepper.h"

class MainVm : public QObject
{
    Q_OBJECT

public:
    MainVm();

public slots:
    void StartSimulation();
    void StopSimulation();
    void UpdateVisualization(bool useGradiant);
    void RequestVisualization(const SimpleValueStepper& stepper, int width, int height);
    void MouseMovedOnPixmap(QPoint mousePos, QSize pixmapSize);

signals:
    void UpdateDone(float minValue, float maxValue);
    void NewVisualization(const QPixmap& pixmap);
    void NewStatusMessage(const QString& message);

private:
    static void SetFixedValues(FloatSurface& surface);

    SimulatorThread* simulatorThread = nullptr;
    bool started = false;

    QSharedPointer<Simulator> simulator;

    QSharedPointer<FloatSurface> surface;
    QSharedPointer<GradientSurface> gradient;

    QElapsedTimer runTimer;
    int frames = 0;
};

#endif // MAINVM_H

#ifndef MAINVM_H
#define MAINVM_H

#include <QSharedPointer>
#include <QElapsedTimer>
#include <QSize>
#include <QThread>

#include "model/simulator.h"
#include "model/simulatorworker.h"
#include "model/gradientsurface.h"
#include "util/simplevaluestepper.h"
#include "graphics/sceneelement.h"

class MainVm : public QObject
{
    Q_OBJECT

public:
    MainVm();

    ~MainVm();

public slots:
    void StartSimulation();
    void StopSimulation();
    void UpdateVisualization(bool useGradiant);
    void RequestVisualization(const SimpleValueStepper& stepper, const QSize& size);
    void MouseMovedOnPixmap(QPoint mousePos, QSize labelSize);
    void MousePressedOnPixmap(QPoint mousePos, Qt::MouseButtons buttons, QSize labelSize);
    void MouseReleasedFromPixmap(QPoint mousePos, Qt::MouseButtons buttons, QSize labelSize);
    void MouseDoubleClickedOnPixmap(QPoint mousePos, Qt::MouseButtons buttons, QSize labelSize);
    void DeleteSelectedElement();

signals:
    void VisualizationAvailable(float minValue, float maxValue);
    void NewVisualization(const QPixmap& pixmap);
    void NewStatusMessage(const QString& message);

    void RunSimulatorWorker();
    void CancelSimulatorWorker();

private:
    void CreateScene();
    void SetFixedValues(FloatSurface& surface);

    SceneElement<float> scene;

    QSharedPointer<DrawingElement<float>> dragginNode = nullptr;

    QThread simulatorThread;
    SimulatorWorker* simulatorWorker = nullptr;

    bool started = false;

    QSharedPointer<Simulator> simulator;

    QSharedPointer<FloatSurface> surface;
    QSharedPointer<GradientSurface> gradient;

    QElapsedTimer runTimer;
    int frames = 0;
};

#endif // MAINVM_H

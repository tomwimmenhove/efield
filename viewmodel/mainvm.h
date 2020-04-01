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
#include "graphics/nodeelement.h"
#include "graphics/lineelement.h"

enum class MouseOperationStatus
{
    Normal,
    DragNode,
    NewNode,
    NewLineP1,
    NewLineP2,
};

class MainVm : public QObject
{
    Q_OBJECT

public:
    MainVm(QWidget* parent = nullptr);

    ~MainVm();

public slots:
    void StartSimulation();
    void StopSimulation();
    void UpdateVisualization(bool useGradiant);
    void RequestVisualization(const SimpleValueStepper& stepper, const QSize& size);
    void MousePressedOnPixmap(const QPoint& mousePos, Qt::MouseButtons buttons, const QSize& labelSize);
    void MouseMovedOnPixmap(const QPoint& mousePos, const QSize& labelSize);
    void MouseReleasedFromPixmap(const QPoint& mousePos, Qt::MouseButtons buttons, const QSize& labelSize);
    void MouseDoubleClickedOnPixmap(const QPoint& mousePos, Qt::MouseButtons buttons, const QSize& labelSize);
    void DeleteSelectedElement();
    void EditSelectedElement();
    void NewNodeElement(const QPoint& mousePos, const QSize& labelSize);
    void NewLineElement(const QPoint& mousePos, const QSize& labelSize);
    void CancelOperation();

signals:
    void VisualizationAvailable(float minValue, float maxValue);
    void NewVisualization(const QPixmap& pixmap);
    void NewStatusMessage(const QString& message);

    void RunSimulatorWorker();
    void CancelSimulatorWorker();

    void MouseOperationStateChanged(MouseOperationStatus state);

private:
    void PlaceNewNodeElement(const QPoint& pointerPosition);
    void CreateScene();
    void SetFixedValues(FloatSurface& surface);
    void EditNode(QSharedPointer<NodeElement<float>> node);
    void EditLine(QSharedPointer<LineElement<float>> line);

    void ActivateOperation(const QPoint& pointerPosition);
    void UpdateStatusBarValue(const QPoint& pointerPosition);

    QWidget* parentWidget;

    MouseOperationStatus mouseOperationState = MouseOperationStatus::Normal;
    QWeakPointer<LineElement<float>> NewLine;

    QPoint nodeSavedPos;

    SceneElement<float> scene;

    //QSharedPointer<DrawingElement<float>> dragginNode = nullptr;

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

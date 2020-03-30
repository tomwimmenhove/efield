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
    void MousePressedOnPixmap(QPoint mousePos, Qt::MouseButtons buttons, QSize labelSize);
    void MouseMovedOnPixmap(QPoint mousePos, QSize labelSize);
    void MouseReleasedFromPixmap(QPoint mousePos, Qt::MouseButtons buttons, QSize labelSize);
    void MouseDoubleClickedOnPixmap(QPoint mousePos, Qt::MouseButtons buttons, QSize labelSize);
    void DeleteSelectedElement();
    void EditSelectedElement();
    void NewNodeElement(const QPoint& mousePos, const QSize& labelSize);
    void NewLineElement(const QPoint& mousePos, const QSize& labelSize);

signals:
    void VisualizationAvailable(float minValue, float maxValue);
    void NewVisualization(const QPixmap& pixmap);
    void NewStatusMessage(const QString& message);

    void RunSimulatorWorker();
    void CancelSimulatorWorker();

private:
    void CreateScene();
    void SetFixedValues(FloatSurface& surface);
    void EditNode(NodeElement<float>* node);
    void EditLine(LineElement<float>* line);

    enum class MouseMoveStatus
    {
        Normal,
        DragNode,
        NewNode,
        NewLineP1,
        NewLineP2,
    };

    QWidget* parentWidget;

    MouseMoveStatus mouseMoveStatus = MouseMoveStatus::Normal;
    QSharedPointer<DrawingElement<float>> NewLine;

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

#ifndef MAINVM_H
#define MAINVM_H

#include <memory>
#include <QSharedPointer>
#include <QElapsedTimer>
#include <QSize>
#include <QThread>

#include "model/project.h"
#include "model/simulator.h"
#include "model/simulatorworker.h"
#include "model/gradientsurface.h"
#include "util/simplevaluestepper.h"
#include "graphics/sceneelement.h"
#include "graphics/nodeelement.h"
#include "graphics/lineelement.h"
#include "mouseoperations/mouseoperation.h"

class MainVm : public QObject
{
    Q_OBJECT

public:
    MainVm(QWidget* parent = nullptr);

    ~MainVm();

public slots:
    void startSimulation();
    void stopSimulation();
    void updateVisualization(bool useGradiant);
    void requestVisualization(const SimpleValueStepper& stepper, const QSize& size);
    void mousePressedOnPixmap(const QPoint& mousePos, Qt::MouseButtons buttons, const QSize& labelSize);
    void mouseMovedOnPixmap(const QPoint& mousePos, const QSize& labelSize);
    void mouseReleasedFromPixmap(const QPoint& mousePos, Qt::MouseButtons buttons, const QSize& labelSize);
    void mouseDoubleClickedOnPixmap(const QPoint& mousePos, Qt::MouseButtons buttons, const QSize& labelSize);
    void deleteSelectedElement();
    void editSelectedElement();
    void newNodeElement(const QPoint& mousePos, const QSize& labelSize);
    void newLineElement(const QPoint& mousePos, const QSize& labelSize);
    void cancelOperation();
    void newSimulation();
    void projectOpen();
    void projectSaveAs();

signals:
    void visualizationAvailable(float minValue, float maxValue);
    void newVisualization(const QPixmap& pixmap);
    void newStatusMessage(const QString& message);
    void runSimulatorWorker();
    void cancelSimulatorWorker();
    void updateMouseCursor(Qt::CursorShape cursor);

private:
#ifdef _OPENMP
    const int numThreads = 6;
#endif

#ifdef QT_DEBUG
    void createScene();
#endif
    void createBorder(float voltage);
    void editNode(NodeElement<float>& node);
    void editLine(LineElement<float>& line);
    void activateOperation(const QPoint& pointerPosition);
    void updateStatusBarValue(const QPoint& pointerPosition);
    void initNewProject(std::unique_ptr<Project>&& newProject);

    QWidget* parentWidget;
    std::unique_ptr<MouseOperation> mouseOperation;
    std::unique_ptr<Project> project;
    QThread simulatorThread;
    SimulatorWorker* simulatorWorker = nullptr;
    bool started = false;
    QSharedPointer<FloatSurface> surface;
    QSharedPointer<GradientSurface> gradient;
    QElapsedTimer runTimer;
    int frames = 0;
};

#endif // MAINVM_H

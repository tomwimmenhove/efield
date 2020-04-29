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
#include "util/undo/undostack.h"
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
    void mouseMovedOnPixmap(const QPoint& mousePos, Qt::MouseButtons buttons, const QSize& labelSize);
    void mouseReleasedFromPixmap(const QPoint& mousePos, Qt::MouseButtons buttons, const QSize& labelSize);
    void mouseDoubleClickedOnPixmap(const QPoint& mousePos, Qt::MouseButtons buttons, const QSize& labelSize);
    void newNodeElement(const QPoint& mousePos, const QSize& labelSize);
    void newLineElement(const QPoint& mousePos, const QSize& labelSize);
    void cancelOperation();
    void newSimulation();
    void projectOpen();
    bool projectSave();
    bool projectSaveAs();
    void closeRequested();
    void undo();
    void redo();
    void selectAll();
    void cut();
    void copy();
    void paste();
    void rotate(double rot);
    void deleteSelectedElement();
    void editSelectedElement();

    void editLine(int id, float defaultValue);
    void editNode(int id, QPoint defaultPosition);

private slots:
    void on_undoStackUpdated(bool canUndo, const QString& undoName, bool canRedo, const QString& redoName, size_t level);

signals:
    void visualizationAvailable(float minValue, float maxValue);
    void newVisualization(const QPixmap& pixmap);
    void newStatusMessage(const QString& message);
    void runSimulatorWorker();
    void cancelSimulatorWorker();
    void updateMouseCursor(Qt::CursorShape cursor);
    void undoStackUpdated(bool canUndo, const QString& undoName, bool canRedo, const QString& redoName);
    void projectStatusUpdate(const QString& filename, bool altered);

private:
#ifdef _OPENMP
    const int numThreads = 6;
#endif

#ifdef QT_DEBUG
    void createScene();
#endif
    void createBorder(float voltage);
    void editElement(DrawingElement<float>& element);
    template<typename T> void activateNewMouseOperation(const QPoint& pointerPosition);
    void postMouseOperation();
    void updateStatusBarValue(const QPoint& pointerPosition);
    void initNewProject(std::unique_ptr<Project>&& newProject);
    bool projectSaveTo(const QString& fileName);
    bool saveIfAltered();

    QWidget* parentWidget;
    QSharedPointer<UndoStack> undoStack;
    std::unique_ptr<MouseOperation> mouseOperation;
    std::unique_ptr<Project> project;
    QSharedPointer<SceneElement<float>> clipBoardScene;
    QThread simulatorThread;
    SimulatorWorker* simulatorWorker = nullptr;
    bool started = false;
    QSharedPointer<FloatSurface> surface;
    QSharedPointer<GradientSurface> gradient;
    QElapsedTimer runTimer;
    int frames = 0;
};

#endif // MAINVM_H

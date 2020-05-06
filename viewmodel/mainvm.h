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
    MainVm();

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
    void newCircleElement(const QPoint& mousePos, const QSize& labelSize);
    void cancelOperation();
    void projectSave();
    void undo();
    void redo();
    void selectAll();
    void cut();
    void copy();
    void paste();
    void rotate(double rot);
    void moveSelection(const QPoint& delta);
    void deleteSelectedElement();
    void editSelectedElement();
    void setLineVoltage(int id, float oldVoltage, float newVoltage);
    void setCircleVoltage(int id, float oldVoltage, float newVoltage);
    void setNodePosition(int id, const QPoint& oldPosition, const QPoint& newPosition);
    void moveSelectionRequested();

    /* Save as */
    bool saveAs(const QString& fileName);

    /* Closing */
    void closeRequested();
    void saveBeforeClose();
    void closeApplication();
    void saveAsBeforeClose(const QString& filename);

    /* Opening */
    void projectOpenRequested();
    void saveBeforeOpen();
    void projectOpen(const QString filename);
    void saveAsBeforeOpen(const QString& filename);

    /* New */
    void newProjectRequested();
    void saveBeforeNewProject();
    void newProject(const QSize& size);
    void saveAsBeforeNewProject(const QString& filename);

private slots:
    void on_undoStackUpdated(bool canUndo, const QString& undoName, bool canRedo, const QString& redoName, size_t level);
    void editVisitor_editLine(int id, float defaultValue);
    void editVisitor_editCircle(int id, float defaultValue);
    void editVisitor_editNode(int id, const QPoint& defaultPosition);

signals:
    void criticalMessage(const QString& topic, const QString& message);
    void visualizationAvailable(float minValue, float maxValue);
    void newVisualization(const QPixmap& pixmap);
    void newStatusMessage(const QString& message);
    void runSimulatorWorker();
    void cancelSimulatorWorker();
    void updateMouseCursor(Qt::CursorShape cursor);
    void undoStackUpdated(bool canUndo, const QString& undoName, bool canRedo, const QString& redoName);
    void projectStatusUpdate(const QString& filename, bool altered);
    void editLine(int id, float defaultValue);
    void editCircle(int id, float defaultValue);
    void editNode(int id, const QPoint& defaultPosition, const QPoint& minPosition, const QPoint& maxPosition);
    void moveSelectionDialog(const QPoint& maxPoint);

    /* Save As */
    void saveDialog();

    /* Closing */
    void askSaveBeforeClose();
    void saveDialogBeforeClose();

    /* Opening */
    void openDialog();
    void askSaveBeforeOpen();
    void saveDialogBeforeOpen();

    /* New */
    void newProjectDialog();
    void askSaveBeforeNewProject();
    void saveDialogBeforeNewProject();

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

#include <cmath>
#include <limits>

#include <QFile>
#include <QApplication>

#include "mainvm.h"
#include "view/pointinputdialog.h"
#include "model/floatsurfacedrawer.h"
#include "visualizer/visualizer.h"
#include "graphics/sceneserializevisitor.h"
#include "graphics/scenedeserializevisitor.h"
#include "mouseoperations/mouseoperations.h"
#include "editdrawingelementvisitor.h"
#include "elementmanipulators.h"
#include <util/undo/linevalueundoitem.h>
#include <util/undo/moveundoitem.h>

MainVm::MainVm()
{
    clipBoardScene =
            QSharedPointer<SceneElement<float>>::create(QSize(std::numeric_limits<int>::max(), std::numeric_limits<int>::max()));

#ifdef QT_DEBUG
    newProject(QSize(601, 601));
    createScene();
#else
    newProject(QSize(256, 256));
#endif

    simulatorThread.start();
}

MainVm::~MainVm()
{
    emit cancelSimulatorWorker();
    simulatorThread.quit();
    simulatorThread.wait();
}

void MainVm::updateVisualization(bool useGradiant)
{
    QSharedPointer<FloatSurface> clonedSurface = project->simulator()->cloneSurface();
    if (useGradiant)
    {
        gradient = QSharedPointer<GradientSurface>(new GradientSurface(*clonedSurface));
        surface = QSharedPointer<FloatSurface>(new FloatSurface(*gradient));
    }
    else
    {
        surface = clonedSurface;
        gradient = nullptr;
    }

    emit visualizationAvailable(surface->minValue(), surface->maxValue());
}

void MainVm::requestVisualization(const SimpleValueStepper& stepper, const QSize& size)
{
    QPixmap pixmapObject = QPixmap::fromImage(Visualizer::imageFromFloatSurface(*surface, stepper));
    QPixmap scaledPixmap = pixmapObject.scaled(size, Qt::KeepAspectRatio);//, Qt::SmoothTransformation);

    QPainter painter(&scaledPixmap);

    if (gradient)
    {
        painter.setPen(Qt::black);
        //painter.setRenderHint(QPainter::Antialiasing);

        Visualizer::paintGradientVectors(painter, *gradient, 15);
    }

    painter.setRenderHint(QPainter::Antialiasing);
    project->scene()->drawAnnotation(painter, surface->size());

    if (++frames % 25 == 0)
        qDebug() << ((float) simulatorWorker->iterations() * 1000.0 / runTimer.elapsed()) << " iterations/sec";

    emit newVisualization(scaledPixmap);
}

void MainVm::updateStatusBarValue(const QPoint& pointerPosition)
{
    if (pointerPosition.x() < 0 || pointerPosition.x() >= surface->width() ||
        pointerPosition.y() < 0 || pointerPosition.y() >= surface->height())
    {
        emit newStatusMessage(QString(tr("[%1, %2]")).arg(pointerPosition.x()).arg(pointerPosition.y()));
        return;
    }

    if (gradient)
    {
        QVector2D v = gradient->value(pointerPosition.x(), pointerPosition.y());

        emit newStatusMessage(QString(tr("Value at [%1, %2]: %3 @%4° (Vector [%5, %6])"))
                              .arg(pointerPosition.x())
                              .arg(pointerPosition.y())
                              .arg(v.length())
                              .arg(atan2(v.y(), v.x()) * 180 / M_PI)
                              .arg(v.x())
                              .arg(v.y()));
    }
    else
    {
        float value = surface->value(pointerPosition.x(), pointerPosition.y());
        emit newStatusMessage(QString(tr("Value at [%1, %2]: %3"))
                              .arg(pointerPosition.x())
                              .arg(pointerPosition.y())
                              .arg(value));
    }
}

void MainVm::cancelOperation()
{
    mouseOperation = std::move(mouseOperation->cancelOperation(std::move(mouseOperation)));
    postMouseOperation();
}

void MainVm::mousePressedOnPixmap(const QPoint& mousePos, Qt::MouseButtons buttons, const QSize& labelSize)
{
    if (buttons == Qt::LeftButton)
    {
        QPoint translated = Geometry::translatePoint(mousePos, labelSize, surface->size(), true);
        mouseOperation = std::move(mouseOperation->mousePressed(std::move(mouseOperation), translated));
        postMouseOperation();
    }
    else if (buttons & Qt::RightButton)
    {
        cancelOperation();
        return;
    }
}

void MainVm::mouseMovedOnPixmap(const QPoint& mousePos, Qt::MouseButtons buttons, const QSize& labelSize)
{
    QPoint translated = Geometry::translatePoint(mousePos, labelSize, surface->size(), true);
    mouseOperation = std::move(mouseOperation->mouseMoved(std::move(mouseOperation), translated, buttons));
    postMouseOperation();

    updateStatusBarValue(translated);
}

void MainVm::mouseReleasedFromPixmap(const QPoint& mousePos, Qt::MouseButtons buttons, const QSize& labelSize)
{
    QPoint translated = Geometry::translatePoint(mousePos, labelSize, surface->size(), true);
    mouseOperation = std::move(mouseOperation->mouseReleased(std::move(mouseOperation), translated, buttons));
    postMouseOperation();
}

void MainVm::mouseDoubleClickedOnPixmap(const QPoint& mousePos, Qt::MouseButtons buttons, const QSize& labelSize)
{
    QPoint translated = Geometry::translatePoint(mousePos, labelSize, surface->size(), true);
    mouseOperation = std::move(mouseOperation->mouseDoubleClicked(std::move(mouseOperation), translated, buttons));
    postMouseOperation();
}

void MainVm::projectSave()
{
    if (project->fileName().isEmpty())
        emit saveDialog();
    else
        saveAs(project->fileName());
}

bool MainVm::saveAs(const QString& fileName)
{
    QFile file(fileName);

    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        emit criticalMessage(tr("Unable to save"),
                             tr("Unable to save %1: %2")
                             .arg(fileName).arg(file.errorString()));

        return false;
    }

    file.write(project->toXmlBytes());
    project->setFileName(fileName);
    project->setAltered(false);
    project->setSavedAtUndoLevel(undoStack->level());

    return true;
}

void MainVm::closeRequested()
{
    if (project->isAltered())
        emit askSaveBeforeClose();
    else
        closeApplication();
}

void MainVm::saveBeforeClose()
{
    if (project->fileName().isEmpty())
        emit saveDialogBeforeClose();
    else
        if (saveAs(project->fileName()))
            closeApplication();
}

void MainVm::closeApplication()
{
    QApplication::quit();
}

void MainVm::saveAsBeforeClose(const QString& filename)
{
    if (saveAs(filename))
        closeApplication();
}

void MainVm::projectOpenRequested()
{
    if (project->isAltered())
        emit askSaveBeforeOpen();
    else
        emit openDialog();
}

void MainVm::saveBeforeOpen()
{
    if (project->fileName().isEmpty())
        emit saveDialogBeforeOpen();
    else
        if (saveAs(project->fileName()))
            emit openDialog();
}

void MainVm::projectOpen(const QString filename)
{
    QFile file(filename);

    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        initNewProject(std::make_unique<Project>(file.readAll()));
        project->setFileName(filename);
    }
    else
        emit criticalMessage(tr("Unable to load"),
                             tr("Unable to load %1: %2")
                             .arg(filename).arg(file.errorString()));
}

void MainVm::saveAsBeforeOpen(const QString& filename)
{
    if (saveAs(filename))
        emit openDialog();
}

void MainVm::newProjectRequested()
{
    if (project->isAltered())
        emit askSaveBeforeNewProject();
    else
        emit newProjectDialog();
}

void MainVm::saveBeforeNewProject()
{
    if (project->fileName().isEmpty())
        emit saveDialogBeforeNewProject();
    else
        if (saveAs(project->fileName()))
            emit newProjectDialog();
}

void MainVm::newProject(const QSize& size)
{
    initNewProject(std::make_unique<Project>(size));
    createBorder(0);
}

void MainVm::saveAsBeforeNewProject(const QString& filename)
{
    if (saveAs(filename))
        emit newProjectDialog();
}

void MainVm::undo()
{
    if (undoStack->canUndo())
    {
        undoStack->undo();
        emit visualizationAvailable(surface->minValue(), surface->maxValue());
    }
}

void MainVm::redo()
{
    if (undoStack->canRedo())
    {
        undoStack->redo();
        emit visualizationAvailable(surface->minValue(), surface->maxValue());
    }
}

void MainVm::on_undoStackUpdated(bool canUndo, const QString& undoName, bool canRedo, const QString& redoName, size_t level)
{
    emit undoStackUpdated(canUndo, undoName, canRedo, redoName);
    project->setAltered(level != project->savedAtUndoLevel());
}

void MainVm::selectAll()
{
    bool update = false;

    for(auto& element: *project->scene())
    {
        if (!element.isHighlighted())
        {
            element.setHighlighted(true);
            update = true;
        }
    }

    if (update)
        emit visualizationAvailable(surface->minValue(), surface->maxValue());
}

void MainVm::cut()
{
    QSharedPointer<SceneElement<float>> scene = project->scene();
    if (scene->numHighlighted() == 0)
        return;

    cancelOperation();

    ElementManipulators manip(project->scene(), undoStack);

    manip.copySelection(clipBoardScene, true);

    if (manip.needsUpdate())
        emit visualizationAvailable(surface->minValue(), surface->maxValue());
}

void MainVm::copy()
{
    QSharedPointer<SceneElement<float>> scene = project->scene();
    if (scene->numHighlighted() == 0)
        return;

    cancelOperation();

    ElementManipulators manip(project->scene(), undoStack);

    manip.copySelection(clipBoardScene, false);

    if (manip.needsUpdate())
        emit visualizationAvailable(surface->minValue(), surface->maxValue());
}

void MainVm::paste()
{
    if (clipBoardScene->size() == 0)
        return;

    ElementManipulators manip(project->scene(), undoStack);

    if (!manip.paste(clipBoardScene))
    {
        emit criticalMessage(tr("Fitting error"),
                             tr("Scene not large enough for clipboard data"));
        return;
    }

    if (manip.needsUpdate())
        emit visualizationAvailable(surface->minValue(), surface->maxValue());
}

void MainVm::rotate(double rot)
{
    QSharedPointer<SceneElement<float>> scene = project->scene();
    if (scene->numHighlighted() == 0)
        return;

    cancelOperation();

    ElementManipulators manip(scene, undoStack);

    if (!manip.rotateSelection(rot))
    {
        emit criticalMessage(tr("Unable to rotate"),
                             tr("Could not rotate selection.\n"
                                "Make sure that all points will stay inside the scene bounds after rotation."));
        return;
    }

    if (manip.needsUpdate())
        emit visualizationAvailable(surface->minValue(), surface->maxValue());
}

void MainVm::deleteSelectedElement()
{
    QSharedPointer<SceneElement<float>> scene = project->scene();
    if (scene->numHighlighted() == 0)
        return;

    cancelOperation();

    ElementManipulators manip(scene, undoStack);

    manip.deleteSelected();

    if (manip.needsUpdate())
        emit visualizationAvailable(surface->minValue(), surface->maxValue());
}

void MainVm::editSelectedElement()
{
    QSharedPointer<SceneElement<float>> scene = project->scene();
    if (scene->numHighlighted() != 1)
        return;

    auto highLighted = scene->findFirstHighLighted();
    Q_ASSERT(highLighted != scene->end());

    cancelOperation();
    editElement(*highLighted);
}

void MainVm::editElement(DrawingElement<float>& element)
{
    EditDrawingElementVisitor v;

    connect(&v, &EditDrawingElementVisitor::editLine, this, &MainVm::editVisitor_editLine);
    connect(&v, &EditDrawingElementVisitor::editNode, this, &MainVm::editVisitor_editNode);

    element.accept(v);
}

void MainVm::editVisitor_editLine(int id, float defaultValue)
{
    emit editLine(id, defaultValue);
}

void MainVm::editVisitor_editNode(int id, const QPoint& defaultPosition)
{
    emit editNode(id, defaultPosition, QPoint(0, 0), QPoint(surface->width() - 1, surface->height() - 1));
}

void MainVm::setLineVoltage(int id, float oldVoltage, float newVoltage)
{
    auto undoItem = std::make_unique<LineValueUndoItem>(project->scene(), id, oldVoltage, newVoltage);
    undoItem->doFunction();
    undoStack->add(std::move(undoItem));
}

void MainVm::setNodePosition(int id, const QPoint& oldPosition, const QPoint& newPosition)
{
    auto undoItem = std::make_unique<MoveUndoItem>(project->scene(), id, oldPosition, newPosition);
    undoItem->doFunction();
    undoStack->add(std::move(undoItem));

    emit visualizationAvailable(surface->minValue(), surface->maxValue());
}

template<typename T>
void MainVm::activateNewMouseOperation(const QPoint& pointerPosition)
{
    /* Cancel any currently active operations */
    mouseOperation = std::move(mouseOperation->cancelOperation(std::move(mouseOperation)));

    /* Instantiate a new NewNodeMouseOperation with the current mouseOperation as
     * parent, and move it to the current one. */
    std::unique_ptr<MouseOperation> newOperation = std::make_unique<T>(std::move(mouseOperation), undoStack, project->scene());

    /* Call it's activate method */
    mouseOperation = std::move(newOperation->activate(std::move(newOperation), pointerPosition));

    postMouseOperation();
}

void MainVm::postMouseOperation()
{
    emit updateMouseCursor(mouseOperation->cursorShape());
    if (mouseOperation->popUpdate())
        emit visualizationAvailable(surface->minValue(), surface->maxValue());
}

void MainVm::newNodeElement(const QPoint& mousePos, const QSize& labelSize)
{
    if (surface)
        activateNewMouseOperation<NewNodeMouseOperation>(
                    Geometry::translatePoint(mousePos, labelSize, surface->size(), true));
}

void MainVm::newLineElement(const QPoint& mousePos, const QSize& labelSize)
{
    if (surface)
        activateNewMouseOperation<NewLineMouseOperation>(
                    Geometry::translatePoint(mousePos, labelSize, surface->size(), true));
}

void MainVm::startSimulation()
{
    if (started)
        return;

    emit runSimulatorWorker();

    runTimer.restart();

    started = true;
}

void MainVm::stopSimulation()
{
    if (!started)
        return;

    emit cancelSimulatorWorker();

    started = false;
}

void MainVm::initNewProject(std::unique_ptr<Project>&& newProject)
{
    if (simulatorWorker)
        simulatorWorker->deleteLater();

    simulatorWorker = new SimulatorWorker(newProject->simulator());

#ifdef _OPENMP
    simulatorWorker->setNumThreads(numThreads);
#endif

    undoStack = QSharedPointer<UndoStack>::create();
    connect(static_cast<UndoStack*>(undoStack.data()), &UndoStack::stackUpdated, this, &MainVm::on_undoStackUpdated);

    connect(&simulatorThread, &QThread::finished, simulatorWorker, &SimulatorWorker::deleteLater);
    connect(this, &MainVm::runSimulatorWorker, simulatorWorker, &SimulatorWorker::run);
    connect(this, &MainVm::cancelSimulatorWorker, simulatorWorker, &SimulatorWorker::stop, Qt::DirectConnection);

    simulatorWorker->moveToThread(&simulatorThread);

    project = std::move(newProject);
    connect(static_cast<Project*>(project.get()), &Project::statusUpdate, this, &MainVm::projectStatusUpdate);
    project->sendStatusUpdate();

    mouseOperation = std::make_unique<NormalMouseOperation>(undoStack, project->scene());
    connect(static_cast<NormalMouseOperation*>(mouseOperation.get()), &NormalMouseOperation::editElement, this, &MainVm::editElement);
}

#ifdef QT_DEBUG
void MainVm::createScene()
{
    QSharedPointer<SceneElement<float>> scene = project->scene();

    SharedNode anodeLeft(scene->newId(), 100, 500);
    SharedNode anodeRight(scene->newId(), 500, 500);

    SharedNode cathodeLeft(scene->newId(), 100, 100);
    SharedNode cathodeRight(scene->newId(), 500, 100);

    scene->add(NodeElement<float>::uniqueElement(anodeRight, scene->sceneSize()));
    scene->add(NodeElement<float>::uniqueElement(anodeLeft, scene->sceneSize()));
    scene->add(NodeElement<float>::uniqueElement(cathodeLeft, scene->sceneSize()));
    scene->add(NodeElement<float>::uniqueElement(cathodeRight, scene->sceneSize()));

    scene->add(LineElement<float>::uniqueElement(scene->newId(), scene->sceneSize(), anodeLeft, anodeRight, 1));
    scene->add(LineElement<float>::uniqueElement(scene->newId(), scene->sceneSize(), cathodeLeft, cathodeRight, -1));
}
#endif

void MainVm::createBorder(float voltage)
{
    QSharedPointer<SceneElement<float>> scene = project->scene();
    QSize size = project->simulator()->size();

    SharedNode topLeft(scene->newId(), 0, size.width() - 1);
    SharedNode topRight(scene->newId(), size.height() - 1, size.width() - 1);
    SharedNode bottomLeft(scene->newId(), 0, 0);
    SharedNode bottomRight(scene->newId(), size.height(), 0);

    scene->add(NodeElement<float>::uniqueElement(topLeft, scene->sceneSize()));
    scene->add(NodeElement<float>::uniqueElement(topRight, scene->sceneSize()));
    scene->add(NodeElement<float>::uniqueElement(bottomLeft, scene->sceneSize()));
    scene->add(NodeElement<float>::uniqueElement(bottomRight, scene->sceneSize()));

    scene->add(LineElement<float>::uniqueElement(scene->newId(), scene->sceneSize(), topLeft, topRight, voltage));
    scene->add(LineElement<float>::uniqueElement(scene->newId(), scene->sceneSize(), bottomLeft, bottomRight, voltage));
    scene->add(LineElement<float>::uniqueElement(scene->newId(), scene->sceneSize(), topLeft, bottomLeft, voltage));
    scene->add(LineElement<float>::uniqueElement(scene->newId(), scene->sceneSize(), topRight, bottomRight, voltage));
}


#include <cmath>

#include <QInputDialog>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

#include "mainvm.h"
#include "pointinputdialog.h"
#include "model/floatsurfacedrawer.h"
#include "visualizer/visualizer.h"
#include "graphics/sceneserializevisitor.h"
#include "graphics/scenedeserializevisitor.h"
#include "mouseoperations/mouseoperations.h"
#include "editdrawingelementvisitor.h"
#include "deletedrawingelementvisitor.h"
#include "util/undo/compositundoitem.h"

MainVm::MainVm(QWidget* parent)
    : QObject(parent), parentWidget(parent)
{
#ifdef QT_DEBUG
    auto newProject = std::make_unique<Project>(QSize(601, 601));
#else
    auto newProject = std::make_unique<Project>(QSize(256, 256));
#endif

    initNewProject(std::move(newProject));
    createBorder(0);

    simulatorThread.start();

#ifdef QT_DEBUG
    createScene();
#endif
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

    frames++;
    if (frames % 25 == 0)
    {
        qDebug() << ((float) simulatorWorker->iterations() * 1000.0 / runTimer.elapsed()) << " iterations/sec";
    }

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
    mouseOperation->cancelOperation(mouseOperation);
    postMouseOperation();
}

void MainVm::mousePressedOnPixmap(const QPoint& mousePos, Qt::MouseButtons buttons, const QSize& labelSize)
{
    if (!surface)
        return;

    if (buttons == Qt::LeftButton)
    {
        QPoint translated = Geometry::translatePoint(mousePos, labelSize, surface->size(), true);
        mouseOperation->mousePressed(mouseOperation, translated);
        postMouseOperation();
    }
    else if (buttons & Qt::RightButton)
    {
        cancelOperation();
        return;
    }
}

void MainVm::mouseMovedOnPixmap(const QPoint& mousePos, const QSize& labelSize)
{
    if (!surface)
        return;

    QPoint translated = Geometry::translatePoint(mousePos, labelSize, surface->size(), true);
    mouseOperation->mouseMoved(mouseOperation, translated);
    postMouseOperation();

    updateStatusBarValue(translated);
}

void MainVm::mouseReleasedFromPixmap(const QPoint& mousePos, Qt::MouseButtons buttons, const QSize& labelSize)
{
    QPoint translated = Geometry::translatePoint(mousePos, labelSize, surface->size(), true);
    mouseOperation->mouseReleased(mouseOperation, translated, buttons);
    postMouseOperation();
}

void MainVm::mouseDoubleClickedOnPixmap(const QPoint& mousePos, Qt::MouseButtons buttons, const QSize& labelSize)
{
    if (!surface)
        return;

    QPoint translated = Geometry::translatePoint(mousePos, labelSize, surface->size(), true);
    mouseOperation->mouseDoubleClicked(mouseOperation, translated, buttons);
    postMouseOperation();
}

void MainVm::newSimulation()
{
    PointInputDialog d("Node coordinates", QPoint(256, 256), QPoint(1, 1), QPoint(1024, 1024), parentWidget);
    if (d.exec() != QDialog::Accepted)
        return;

    initNewProject(std::make_unique<Project>(d.size()));

    createBorder(0);
}

void MainVm::projectOpen()
{
    QString fileName = QFileDialog::getOpenFileName(parentWidget, tr("Open Project"), "",
                                                    tr("E-Field Sim files (*.efs)"));

    if (!fileName.isEmpty())
    {
        QFile file(fileName);

        if(file.open(QIODevice::ReadOnly | QIODevice::Text))
            initNewProject(std::make_unique<Project>(file.readAll()));
        else
            QMessageBox::critical(parentWidget, "Unable to load",
                                  QString("Unable to load %1: %2")
                                  .arg(fileName).arg(file.errorString()));
    }
}

void MainVm::projectSaveAs()
{
    QString fileName = QFileDialog::getSaveFileName(parentWidget, tr("Open Project"), "",
                                                    tr("E-Field Sim files (*.efs)"));

    if (!fileName.isEmpty())
    {
        QFile file(fileName);

        if(file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
            file.write(project->toXmlBytes());
        else
            QMessageBox::critical(parentWidget, "Unable to save",
                                  QString("Unable to save %1: %2")
                                  .arg(fileName).arg(file.errorString()));
    }
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

void MainVm::deleteSelectedElement()
{
    if (!surface)
        return;

    QSharedPointer<SceneElement<float>> scene = project->scene();
    if (scene->numHighlighted() == 0)
        return;

    cancelOperation();

    QSharedPointer<UndoStack> nestedUndoStack = QSharedPointer<UndoStack>::create();

    bool update = false;
    int n;
    do {
        n = 0;
        for (auto it = scene->begin(); it != scene->end(); ++it)
        {
            if (it->isHighlighted() && !it->isInUse())
            {
                update |= DeleteDrawingElementVisitor::deleteElement(nestedUndoStack, scene, *it);
                n++;
                break;
            }
        }
    } while (n != 0);

    undoStack->add(std::make_unique<CompositUndoItem>(scene, nestedUndoStack, "Delete selected"));

    if (update)
        emit visualizationAvailable(surface->minValue(), surface->maxValue());
}

void MainVm::editElement(DrawingElement<float>& element)
{
    if (EditDrawingElementVisitor::editElement(parentWidget, undoStack, project->scene(), element, surface))
        emit visualizationAvailable(surface->minValue(), surface->maxValue());
}

void MainVm::editSelectedElement()
{
    if (!surface)
        return;

    QSharedPointer<SceneElement<float>> scene = project->scene();
    if (scene->numHighlighted() != 1)
        return;

    auto highLighted = scene->findFirstHighLighted();
    Q_ASSERT(highLighted != scene->end());

    cancelOperation();
    editElement(*highLighted);
}

template<typename T>
void MainVm::activateNewMouseOperation(const QPoint& pointerPosition)
{
    /* Cancel any currently active operations */
    mouseOperation->cancelOperation(mouseOperation);

    /* Instantiate a new NewNodeMouseOperation with the current mouseOperation as
     * parent, and move it to the current one. */
    mouseOperation = std::move(std::make_unique<T>(std::move(mouseOperation), undoStack, project->scene()));

    /* Call it's activate method */
    mouseOperation->activate(mouseOperation, pointerPosition);

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
    connect(static_cast<UndoStack*>(undoStack.data()), &UndoStack::stackUpdated, this, &MainVm::undoStackUpdated);

    connect(&simulatorThread, &QThread::finished, simulatorWorker, &SimulatorWorker::deleteLater);
    connect(this, &MainVm::runSimulatorWorker, simulatorWorker, &SimulatorWorker::run);
    connect(this, &MainVm::cancelSimulatorWorker, simulatorWorker, &SimulatorWorker::stop, Qt::DirectConnection);

    simulatorWorker->moveToThread(&simulatorThread);

    project = std::move(newProject);

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

    scene->add(NodeElement<float>::uniqueElement(anodeRight, scene->sceneBounds()));
    scene->add(NodeElement<float>::uniqueElement(anodeLeft, scene->sceneBounds()));
    scene->add(NodeElement<float>::uniqueElement(cathodeLeft, scene->sceneBounds()));
    scene->add(NodeElement<float>::uniqueElement(cathodeRight, scene->sceneBounds()));

    scene->add(LineElement<float>::uniqueElement(scene->newId(), scene->sceneBounds(), anodeLeft, anodeRight, 1));
    scene->add(LineElement<float>::uniqueElement(scene->newId(), scene->sceneBounds(), cathodeLeft, cathodeRight, -1));
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

    scene->add(NodeElement<float>::uniqueElement(topLeft, scene->sceneBounds()));
    scene->add(NodeElement<float>::uniqueElement(topRight, scene->sceneBounds()));
    scene->add(NodeElement<float>::uniqueElement(bottomLeft, scene->sceneBounds()));
    scene->add(NodeElement<float>::uniqueElement(bottomRight, scene->sceneBounds()));

    scene->add(LineElement<float>::uniqueElement(scene->newId(), scene->sceneBounds(), topLeft, topRight, voltage));
    scene->add(LineElement<float>::uniqueElement(scene->newId(), scene->sceneBounds(), bottomLeft, bottomRight, voltage));
    scene->add(LineElement<float>::uniqueElement(scene->newId(), scene->sceneBounds(), topLeft, bottomLeft, voltage));
    scene->add(LineElement<float>::uniqueElement(scene->newId(), scene->sceneBounds(), topRight, bottomRight, voltage));
}


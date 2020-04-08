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

MainVm::MainVm(QWidget* parent)
    : QObject(parent), parentWidget(parent)
{
#ifdef QT_DEBUG
    std::unique_ptr<Project> newProject = std::make_unique<Project>(QSize(601, 601));
#else
    std::unique_ptr<Project> newProject = std::make_unique<Project>(QSize(256, 256));
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
    QSharedPointer<FloatSurface> clonedSurface = project->sharedSimulator()->cloneSurface();
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
    project->sceneRef().drawAnnotation(painter, surface->size());

    frames++;
    if (frames % 25 == 0)
    {
        qDebug() << ((float) simulatorWorker->iterations() * 1000.0 / runTimer.elapsed()) << " iterations/sec";
    }

    emit newVisualization(scaledPixmap);
}

void MainVm::updateStatusBarValue(const QPoint& pointerPosition)
{
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

void MainVm::activateOperation(const QPoint& pointerPosition)
{
    SceneElement<float>& scene = project->sceneRef();

    auto closest = scene.closestElement(pointerPosition);
    auto highLighted = scene.findHighLighted();

    MouseOperationStatus oldState = mouseOperationState;

    switch(mouseOperationState)
    {
        case MouseOperationStatus::Normal:
        {
            if (highLighted != scene.end() && closest == highLighted &&
                highLighted->elementType() == drawingElementType::Node)
            {
                mouseOperationState = MouseOperationStatus::DragNode;
                NodeElement<float>& node = static_cast<NodeElement<float>&>(*highLighted);
                nodeSavedPos = node.node();
                break;
            }

            scene.highlight(closest);
            break;
        }
        case MouseOperationStatus::DragNode:
            scene.highlight(scene.end());
            mouseOperationState = MouseOperationStatus::Normal;
            break;
        case MouseOperationStatus::NewNode:
            placeNewNodeElement(pointerPosition);
            break;
        case MouseOperationStatus::NewLineP1:
            if (closest != scene.end() && highLighted != scene.end())
            {
                Q_ASSERT(highLighted->elementType() == drawingElementType::Node);
                NodeElement<float>& startNode = static_cast<NodeElement<float>&>(*highLighted);
                SharedNode sharedStartNode = startNode.node();
                SharedNode sharedEndNode = SharedNode(pointerPosition);
                scene.add(std::move(LineElement<float>::UniqueElement(sharedStartNode, sharedEndNode, 0)));

                mouseOperationState = MouseOperationStatus::NewLineP2;
                break;
            }
            return;
        case MouseOperationStatus::NewLineP2:
        {
            if (highLighted != scene.end())
            {
                /* Set the definitive end point for the new line segment */
                Q_ASSERT(highLighted->elementType() == drawingElementType::Node);
                NodeElement<float>& endNode = static_cast<NodeElement<float>&>(*highLighted);
                Q_ASSERT(scene.back().elementType() == drawingElementType::Line);
                LineElement<float>& newLine = static_cast<LineElement<float>&>(scene.back());

                if (endNode.node()->identifier() != newLine.point1()->identifier())
                {
                    newLine.setPoint2(endNode.node());
                    mouseOperationState = MouseOperationStatus::NewLineP1;
                }
            }
            scene.highlight(scene.end());
            break;
        }
    }

    if (oldState != mouseOperationState)
        emit mouseOperationStateChanged(mouseOperationState);

    emit visualizationAvailable(surface->minValue(), surface->maxValue());
}

void MainVm::cancelOperation()
{
    SceneElement<float>& scene = project->sceneRef();

    auto highLighted = scene.findHighLighted();

    MouseOperationStatus oldState = mouseOperationState;

    switch(mouseOperationState)
    {
        case MouseOperationStatus::Normal:
            return;
        case MouseOperationStatus::DragNode:
        {
            Q_ASSERT(highLighted->elementType() == drawingElementType::Node);
            NodeElement<float>& node = static_cast<NodeElement<float>&>(*highLighted);
            node.node().setPoint(nodeSavedPos);
            mouseOperationState = MouseOperationStatus::Normal;
            break;
        }
        case MouseOperationStatus::NewNode:
            scene.highlight(scene.end());
            if (highLighted != scene.end())
                scene.remove(highLighted);
            mouseOperationState = MouseOperationStatus::Normal;
            break;
        case MouseOperationStatus::NewLineP1:
            scene.highlight(scene.end());
            mouseOperationState = MouseOperationStatus::Normal;
            break;
        case MouseOperationStatus::NewLineP2:
            Q_ASSERT(scene.back().elementType() == drawingElementType::Line);
            scene.pop();
            mouseOperationState = MouseOperationStatus::Normal;
            scene.highlight(scene.end());
            break;
    }

    if (oldState != mouseOperationState)
        emit mouseOperationStateChanged(mouseOperationState);

    emit visualizationAvailable(surface->minValue(), surface->maxValue());
}


void MainVm::mousePressedOnPixmap(const QPoint& mousePos, Qt::MouseButtons buttons, const QSize& labelSize)
{
    if (!surface)
        return;

    QPoint translated = Geometry::translatePoint(mousePos, labelSize, surface->size(), true);
    if (translated.x() < 0 || translated.x() >= surface->width() || translated.y() < 0 || translated.y() >= surface->height())
    {
        emit newStatusMessage(QString(tr("[%1, %2]")).arg(translated.x()).arg(translated.y()));
        return;
    }

    if (buttons == Qt::LeftButton)
        activateOperation(translated);
    else if (buttons & Qt::RightButton)
        cancelOperation();
}

void MainVm::mouseMovedOnPixmap(const QPoint& mousePos, const QSize& labelSize)
{
    if (!surface)
        return;

    QPoint translated = Geometry::translatePoint(mousePos, labelSize, surface->size(), true);
    if (translated.x() < 0 || translated.x() >= surface->width() || translated.y() < 0 || translated.y() >= surface->height())
    {
        emit newStatusMessage(QString(tr("[%1, %2]")).arg(translated.x()).arg(translated.y()));
        return;
    }

    SceneElement<float>& scene = project->sceneRef();
    auto closest = scene.closestElement(translated, 15, drawingElementType::Node);

    switch(mouseOperationState)
    {
        case MouseOperationStatus::Normal:
        {
            updateStatusBarValue(translated);
            return;
        }
        case MouseOperationStatus::DragNode:
        case MouseOperationStatus::NewNode:
        {
            auto highLighted = scene.findHighLighted();
            Q_ASSERT(highLighted != scene.end());
            Q_ASSERT(highLighted->elementType() == drawingElementType::Node);
            NodeElement<float>& node = static_cast<NodeElement<float>&>(*highLighted);
            node.node().setPoint(translated);
            break;
        }
        case MouseOperationStatus::NewLineP1:
        {
            /* Highlight close nodes */
            if (closest == scene.end())
            {
                scene.highlight(scene.end());
                break;
            }

            if (closest->elementType() == drawingElementType::Node)
            {
                scene.highlight(closest);
                break;
            }

            return;
        }
        case MouseOperationStatus::NewLineP2:
        {
            Q_ASSERT(scene.back().elementType() == drawingElementType::Line);
            LineElement<float>& newLine = static_cast<LineElement<float>&>(scene.back());

            if (closest == scene.end())
                scene.highlight(scene.end());
            else if (closest->elementType() == drawingElementType::Node &&
                     static_cast<NodeElement<float>&>(*closest).node()->identifier() != newLine.point1()->identifier())
                scene.highlight(closest);

            newLine.point2().setPoint(translated);
            break;
        }
    }

    emit visualizationAvailable(surface->minValue(), surface->maxValue());
}

void MainVm::mouseReleasedFromPixmap(const QPoint&, Qt::MouseButtons buttons, const QSize&)
{
    if (buttons == Qt::RightButton)
        return;

    switch(mouseOperationState)
    {
        case MouseOperationStatus::Normal:
            break;
        case MouseOperationStatus::DragNode:
            mouseOperationState = MouseOperationStatus::Normal;
            emit mouseOperationStateChanged(mouseOperationState);
            break;
        case MouseOperationStatus::NewNode:
            break;
        case MouseOperationStatus::NewLineP1:
            break;
        case MouseOperationStatus::NewLineP2:
            break;
    }
}

void MainVm::mouseDoubleClickedOnPixmap(const QPoint& mousePos, Qt::MouseButtons, const QSize& labelSize)
{
    if (!surface)
        return;

    QPoint translated = Geometry::translatePoint(mousePos, labelSize, surface->size(), true);
    if (translated.x() < 0 || translated.x() >= surface->width() || translated.y() < 0 || translated.y() >= surface->height())
    {
        emit newStatusMessage(QString(tr("[%1, %2]")).arg(translated.x()).arg(translated.y()));
        return;
    }

    SceneElement<float>& scene = project->sceneRef();
    auto closest = scene.closestElement(translated);
    if (closest == scene.end())
        return;

    switch (closest->elementType())
    {
        case drawingElementType::None:
        case drawingElementType::Scene:
            break;
        case drawingElementType::Node:
            editNode(static_cast<NodeElement<float>&>(*closest));
            break;
        case drawingElementType::Line:
            editLine(static_cast<LineElement<float>&>(*closest));
            break;
    }
}

void MainVm::eewSimulation()
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

void MainVm::deleteSelectedElement()
{
    if (!surface)
        return;

    SceneElement<float>& scene = project->sceneRef();

    auto highLighted = scene.findHighLighted();
    if (highLighted == scene.end())
        return;

    if (highLighted->elementType() == drawingElementType::Node)
    {
        NodeElement<float>& node = static_cast<NodeElement<float>&>(*highLighted);

        // Can't delete nodes that are used by other elements */
        if (node.node()->refCounter() > 0)
            return;
    }

    scene.remove(highLighted);

    emit visualizationAvailable(surface->minValue(), surface->maxValue());
}

void MainVm::editNode(NodeElement<float>& node)
{
    SharedNode sharedNode = node.node();

    PointInputDialog d("Node coordinates", sharedNode, QPoint(0, 0), QPoint(surface->width() - 1, surface->height() - 1), parentWidget);
    if (d.exec() != QDialog::Accepted)
        return;

    sharedNode.setPoint(d.point());

    emit visualizationAvailable(surface->minValue(), surface->maxValue());
}

void MainVm::editLine(LineElement<float>& line)
{
    bool ok;
    int def = line.value();

    int volt = QInputDialog::getInt(parentWidget, tr("Edit line"),
                                    tr("Voltage: "),  def, -2147483647, 2147483647, 1, &ok);
    if (ok)
        line.setValue(volt);
}

void MainVm::editSelectedElement()
{
    if (!surface || mouseOperationState != MouseOperationStatus::Normal)
        return;

    SceneElement<float>& scene = project->sceneRef();

    auto highLighted = scene.findHighLighted();
    if (highLighted == scene.end())
        return;

    switch (highLighted->elementType())
    {
        case drawingElementType::None:
        case drawingElementType::Scene:
            break;
        case drawingElementType::Node:
            editNode(static_cast<NodeElement<float>&>(*highLighted));
            break;
        case drawingElementType::Line:
            editLine(static_cast<LineElement<float>&>(*highLighted));
            break;
    }
}

void MainVm::placeNewNodeElement(const QPoint& pointerPosition)
{
    SceneElement<float>& scene = project->sceneRef();
    scene.highlight(scene.end());

    std::unique_ptr<DrawingElement<float>> newNode = NodeElement<float>::uniqueElement(SharedNode(pointerPosition));
    newNode->setHighlighted(true);
    scene.add(std::move(newNode));
}

void MainVm::newNodeElement(const QPoint& mousePos, const QSize& labelSize)
{
    if (!surface)
        return;

    cancelOperation();

    QPoint translated = Geometry::translatePoint(mousePos, labelSize, surface->size(), true);

    placeNewNodeElement(translated);

    mouseOperationState = MouseOperationStatus::NewNode;
    emit mouseOperationStateChanged(mouseOperationState);
}

void MainVm::newLineElement(const QPoint&, const QSize&)
{
    if (!surface)
        return;

    cancelOperation();

    mouseOperationState = MouseOperationStatus::NewLineP1;
    emit mouseOperationStateChanged(mouseOperationState);
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

    simulatorWorker = new SimulatorWorker(newProject->sharedSimulator());

#ifdef _OPENMP
    simulatorWorker->setNumThreads(numThreads);
#endif

    connect(&simulatorThread, &QThread::finished, simulatorWorker, &SimulatorWorker::deleteLater);
    connect(this, &MainVm::runSimulatorWorker, simulatorWorker, &SimulatorWorker::run);
    connect(this, &MainVm::cancelSimulatorWorker, simulatorWorker, &SimulatorWorker::stop, Qt::DirectConnection);

    simulatorWorker->moveToThread(&simulatorThread);

    project = std::move(newProject);
}

#ifdef QT_DEBUG
void MainVm::createScene()
{
    SceneElement<float>& scene = project->sceneRef();

    SharedNode anodeLeft(100, 500);
    SharedNode anodeRight(500, 500);

    SharedNode cathodeLeft(100, 100);
    SharedNode cathodeRight(500, 100);

    scene.add(NodeElement<float>::uniqueElement(anodeRight));
    scene.add(NodeElement<float>::uniqueElement(anodeLeft));
    scene.add(NodeElement<float>::uniqueElement(cathodeLeft));
    scene.add(NodeElement<float>::uniqueElement(cathodeRight));

    scene.add(LineElement<float>::UniqueElement(anodeLeft, anodeRight, 1));
    scene.add(LineElement<float>::UniqueElement(cathodeLeft, cathodeRight, -1));
}
#endif

void MainVm::createBorder(float voltage)
{
    SceneElement<float>& scene = project->sceneRef();
    QSize size = project->sharedSimulator()->size();

    SharedNode topLeft(0, size.width() - 1);
    SharedNode topRight(size.height() - 1, size.width() - 1);
    SharedNode bottomLeft(0, 0);
    SharedNode bottomRight(size.height(), 0);

    scene.add(NodeElement<float>::uniqueElement(topLeft));
    scene.add(NodeElement<float>::uniqueElement(topRight));
    scene.add(NodeElement<float>::uniqueElement(bottomLeft));
    scene.add(NodeElement<float>::uniqueElement(bottomRight));

    scene.add(LineElement<float>::UniqueElement(topLeft, topRight, voltage));
    scene.add(LineElement<float>::UniqueElement(bottomLeft, bottomRight, voltage));
    scene.add(LineElement<float>::UniqueElement(topLeft, bottomLeft, voltage));
    scene.add(LineElement<float>::UniqueElement(topRight, bottomRight, voltage));
}


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

    InitNewProject(std::move(newProject));
    CreateBorder(0);

    simulatorThread.start();

#ifdef QT_DEBUG
    CreateScene();
#endif
}

MainVm::~MainVm()
{
    emit CancelSimulatorWorker();
    simulatorThread.quit();
    simulatorThread.wait();
}

void MainVm::UpdateVisualization(bool useGradiant)
{
    QSharedPointer<FloatSurface> clonedSurface = project->SharedSimulator()->CloneSurface();
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

    emit VisualizationAvailable(surface->MinValue(), surface->MaxValue());
}

void MainVm::RequestVisualization(const SimpleValueStepper& stepper, const QSize& size)
{
    QPixmap pixmapObject = QPixmap::fromImage(Visualizer::QImageFromFloatSurface(*surface, stepper));
    QPixmap scaledPixmap = pixmapObject.scaled(size, Qt::KeepAspectRatio);//, Qt::SmoothTransformation);

    QPainter painter(&scaledPixmap);

    if (gradient)
    {
        painter.setPen(Qt::black);
        //painter.setRenderHint(QPainter::Antialiasing);

        Visualizer::PaintGradientVectors(painter, *gradient, 15);
    }

    painter.setRenderHint(QPainter::Antialiasing);
    project->SceneRef().DrawAnnotation(painter, surface->Size());

    frames++;
    if (frames % 25 == 0)
    {
        qDebug() << ((float) simulatorWorker->Iterations() * 1000.0 / runTimer.elapsed()) << " iterations/sec";
    }

    emit NewVisualization(scaledPixmap);
}

void MainVm::UpdateStatusBarValue(const QPoint& pointerPosition)
{
    if (gradient)
    {
        QVector2D v = gradient->XYValue(pointerPosition.x(), pointerPosition.y());

        emit NewStatusMessage(QString(tr("Value at [%1, %2]: %3 @%4° (Vector [%5, %6])"))
                              .arg(pointerPosition.x())
                              .arg(pointerPosition.y())
                              .arg(v.length())
                              .arg(atan2(v.y(), v.x()) * 180 / M_PI)
                              .arg(v.x())
                              .arg(v.y()));
    }
    else
    {
        float value = surface->XYValue(pointerPosition.x(), pointerPosition.y());
        emit NewStatusMessage(QString(tr("Value at [%1, %2]: %3"))
                              .arg(pointerPosition.x())
                              .arg(pointerPosition.y())
                              .arg(value));
    }
}

void MainVm::ActivateOperation(const QPoint& pointerPosition)
{
    SceneElement<float>& scene = project->SceneRef();

    auto closest = scene.ClosestElement(pointerPosition);
    auto highLighted = scene.FindHighLigted();

    MouseOperationStatus oldState = mouseOperationState;

    switch(mouseOperationState)
    {
        case MouseOperationStatus::Normal:
        {
            if (highLighted != scene.end() && closest == highLighted &&
                highLighted->ElementType() == DrawingElementType::Node)
            {
                mouseOperationState = MouseOperationStatus::DragNode;
                NodeElement<float>& node = static_cast<NodeElement<float>&>(*highLighted);
                nodeSavedPos = node.Node();
                break;
            }

            scene.Highlight(closest);
            break;
        }
        case MouseOperationStatus::DragNode:
            scene.Highlight(scene.end());
            mouseOperationState = MouseOperationStatus::Normal;
            break;
        case MouseOperationStatus::NewNode:
            PlaceNewNodeElement(pointerPosition);
            break;
        case MouseOperationStatus::NewLineP1:
            if (closest != scene.end() && highLighted != scene.end())
            {
                Q_ASSERT(highLighted->ElementType() == DrawingElementType::Node);
                NodeElement<float>& startNode = static_cast<NodeElement<float>&>(*highLighted);
                SharedNode sharedStartNode = startNode.Node();
                SharedNode sharedEndNode = SharedNode(pointerPosition);
                scene.Add(std::move(LineElement<float>::UniqueElement(sharedStartNode, sharedEndNode, 0)));

                mouseOperationState = MouseOperationStatus::NewLineP2;
                break;
            }
            return;
        case MouseOperationStatus::NewLineP2:
        {
            if (highLighted != scene.end())
            {
                /* Set the definitive end point for the new line segment */
                Q_ASSERT(highLighted->ElementType() == DrawingElementType::Node);
                NodeElement<float>& endNode = static_cast<NodeElement<float>&>(*highLighted);
                Q_ASSERT(scene.Back().ElementType() == DrawingElementType::Line);
                LineElement<float>& newLine = static_cast<LineElement<float>&>(scene.Back());

                if (endNode.Node()->Id() != newLine.P1()->Id())
                {
                    newLine.SetP2(endNode.Node());
                    mouseOperationState = MouseOperationStatus::NewLineP1;
                }
            }
            scene.Highlight(scene.end());
            break;
        }
    }

    if (oldState != mouseOperationState)
        emit MouseOperationStateChanged(mouseOperationState);

    emit VisualizationAvailable(surface->MinValue(), surface->MaxValue());
}

void MainVm::CancelOperation()
{
    SceneElement<float>& scene = project->SceneRef();

    auto highLighted = scene.FindHighLigted();

    MouseOperationStatus oldState = mouseOperationState;

    switch(mouseOperationState)
    {
        case MouseOperationStatus::Normal:
            return;
        case MouseOperationStatus::DragNode:
        {
            Q_ASSERT(highLighted->ElementType() == DrawingElementType::Node);
            NodeElement<float>& node = static_cast<NodeElement<float>&>(*highLighted);
            node.Node().SetPosition(nodeSavedPos);
            mouseOperationState = MouseOperationStatus::Normal;
            break;
        }
        case MouseOperationStatus::NewNode:
            scene.Highlight(scene.end());
            if (highLighted != scene.end())
                scene.Remove(highLighted);
            mouseOperationState = MouseOperationStatus::Normal;
            break;
        case MouseOperationStatus::NewLineP1:
            scene.Highlight(scene.end());
            mouseOperationState = MouseOperationStatus::Normal;
            break;
        case MouseOperationStatus::NewLineP2:
            Q_ASSERT(scene.Back().ElementType() == DrawingElementType::Line);
            scene.PopBack();
            mouseOperationState = MouseOperationStatus::Normal;
            scene.Highlight(scene.end());
            break;
    }

    if (oldState != mouseOperationState)
        emit MouseOperationStateChanged(mouseOperationState);

    emit VisualizationAvailable(surface->MinValue(), surface->MaxValue());
}


void MainVm::MousePressedOnPixmap(const QPoint& mousePos, Qt::MouseButtons buttons, const QSize& labelSize)
{
    if (!surface)
        return;

    QPoint translated = Geometry::TranslatePoint(mousePos, labelSize, surface->Size(), true);
    if (translated.x() < 0 || translated.x() >= surface->Width() || translated.y() < 0 || translated.y() >= surface->Height())
    {
        emit NewStatusMessage(QString(tr("[%1, %2]")).arg(translated.x()).arg(translated.y()));
        return;
    }

    if (buttons == Qt::LeftButton)
        ActivateOperation(translated);
    else if (buttons & Qt::RightButton)
        CancelOperation();
}

void MainVm::MouseMovedOnPixmap(const QPoint& mousePos, const QSize& labelSize)
{
    if (!surface)
        return;

    QPoint translated = Geometry::TranslatePoint(mousePos, labelSize, surface->Size(), true);
    if (translated.x() < 0 || translated.x() >= surface->Width() || translated.y() < 0 || translated.y() >= surface->Height())
    {
        emit NewStatusMessage(QString(tr("[%1, %2]")).arg(translated.x()).arg(translated.y()));
        return;
    }

    SceneElement<float>& scene = project->SceneRef();
    auto closest = scene.ClosestElement(translated, 15, DrawingElementType::Node);

    switch(mouseOperationState)
    {
        case MouseOperationStatus::Normal:
        {
            UpdateStatusBarValue(translated);
            return;
        }
        case MouseOperationStatus::DragNode:
        case MouseOperationStatus::NewNode:
        {
            auto highLighted = scene.FindHighLigted();
            Q_ASSERT(highLighted != scene.end());
            Q_ASSERT(highLighted->ElementType() == DrawingElementType::Node);
            NodeElement<float>& node = static_cast<NodeElement<float>&>(*highLighted);
            node.Node().SetPosition(translated);
            break;
        }
        case MouseOperationStatus::NewLineP1:
        {
            /* Highlight close nodes */
            if (closest == scene.end())
            {
                scene.Highlight(scene.end());
                break;
            }

            if (closest->ElementType() == DrawingElementType::Node)
            {
                scene.Highlight(closest);
                break;
            }

            return;
        }
        case MouseOperationStatus::NewLineP2:
        {
            Q_ASSERT(scene.Back().ElementType() == DrawingElementType::Line);
            LineElement<float>& newLine = static_cast<LineElement<float>&>(scene.Back());

            if (closest == scene.end())
                scene.Highlight(scene.end());
            else if (closest->ElementType() == DrawingElementType::Node &&
                     static_cast<NodeElement<float>&>(*closest).Node()->Id() != newLine.P1()->Id())
                scene.Highlight(closest);

            newLine.P2().SetPosition(translated);
            break;
        }
    }

    emit VisualizationAvailable(surface->MinValue(), surface->MaxValue());
}

void MainVm::MouseReleasedFromPixmap(const QPoint&, Qt::MouseButtons buttons, const QSize&)
{
    if (buttons == Qt::RightButton)
        return;

    switch(mouseOperationState)
    {
        case MouseOperationStatus::Normal:
            break;
        case MouseOperationStatus::DragNode:
            mouseOperationState = MouseOperationStatus::Normal;
            emit MouseOperationStateChanged(mouseOperationState);
            break;
        case MouseOperationStatus::NewNode:
            break;
        case MouseOperationStatus::NewLineP1:
            break;
        case MouseOperationStatus::NewLineP2:
            break;
    }
}

void MainVm::MouseDoubleClickedOnPixmap(const QPoint& mousePos, Qt::MouseButtons, const QSize& labelSize)
{
    if (!surface)
        return;

    QPoint translated = Geometry::TranslatePoint(mousePos, labelSize, surface->Size(), true);
    if (translated.x() < 0 || translated.x() >= surface->Width() || translated.y() < 0 || translated.y() >= surface->Height())
    {
        emit NewStatusMessage(QString(tr("[%1, %2]")).arg(translated.x()).arg(translated.y()));
        return;
    }

    SceneElement<float>& scene = project->SceneRef();
    auto closest = scene.ClosestElement(translated);
    if (closest == scene.end())
        return;

    switch (closest->ElementType())
    {
        case DrawingElementType::None:
        case DrawingElementType::Scene:
            break;
        case DrawingElementType::Node:
            EditNode(static_cast<NodeElement<float>&>(*closest));
            break;
        case DrawingElementType::Line:
            EditLine(static_cast<LineElement<float>&>(*closest));
            break;
    }
}

void MainVm::NewSimulation()
{
    PointInputDialog d("Node coordinates", QPoint(256, 256), QPoint(1, 1), QPoint(1024, 1024), parentWidget);
    if (d.exec() != QDialog::Accepted)
        return;

    InitNewProject(std::make_unique<Project>(d.Size()));

    CreateBorder(0);
}

void MainVm::ProjectOpen()
{
    QString fileName = QFileDialog::getOpenFileName(parentWidget, tr("Open Project"), "",
                                                    tr("E-Field Sim files (*.efs)"));

    if (!fileName.isEmpty())
    {
        QFile file(fileName);

        if(file.open(QIODevice::ReadOnly | QIODevice::Text))
            InitNewProject(std::make_unique<Project>(file.readAll()));
        else
            QMessageBox::critical(parentWidget, "Unable to load",
                                  QString("Unable to load %1: %2")
                                  .arg(fileName).arg(file.errorString()));

    }
}

void MainVm::ProjectSaveAs()
{
    QString fileName = QFileDialog::getSaveFileName(parentWidget, tr("Open Project"), "",
                                                    tr("E-Field Sim files (*.efs)"));

    if (!fileName.isEmpty())
    {
        QFile file(fileName);

        if(file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
            file.write(project->ToXmlBytes());
        else
            QMessageBox::critical(parentWidget, "Unable to save",
                                  QString("Unable to save %1: %2")
                                  .arg(fileName).arg(file.errorString()));
    }
}

void MainVm::DeleteSelectedElement()
{
    if (!surface)
        return;

    SceneElement<float>& scene = project->SceneRef();

    auto highLighted = scene.FindHighLigted();
    if (highLighted == scene.end())
        return;

    if (highLighted->ElementType() == DrawingElementType::Node)
    {
        NodeElement<float>& node = static_cast<NodeElement<float>&>(*highLighted);

        // Can't delete nodes that are used by other elements */
        if (node.Node()->RefCount() > 0)
            return;
    }

    scene.Remove(highLighted);

    emit VisualizationAvailable(surface->MinValue(), surface->MaxValue());
}

void MainVm::EditNode(NodeElement<float>& node)
{
    SharedNode sharedNode = node.Node();

    PointInputDialog d("Node coordinates", sharedNode, QPoint(0, 0), QPoint(surface->Width() - 1, surface->Height() - 1), parentWidget);
    if (d.exec() != QDialog::Accepted)
        return;

    sharedNode.SetPosition(d.Point());

    emit VisualizationAvailable(surface->MinValue(), surface->MaxValue());
}

void MainVm::EditLine(LineElement<float>& line)
{
    bool ok;
    int def = line.Value();

    int volt = QInputDialog::getInt(parentWidget, tr("Edit line"),
                                    tr("Voltage: "),  def, -2147483647, 2147483647, 1, &ok);
    if (ok)
        line.SetValue(volt);
}

void MainVm::EditSelectedElement()
{
    if (!surface || mouseOperationState != MouseOperationStatus::Normal)
        return;

    SceneElement<float>& scene = project->SceneRef();

    auto highLighted = scene.FindHighLigted();
    if (highLighted == scene.end())
        return;

    switch (highLighted->ElementType())
    {
        case DrawingElementType::None:
        case DrawingElementType::Scene:
            break;
        case DrawingElementType::Node:
            EditNode(static_cast<NodeElement<float>&>(*highLighted));
            break;
        case DrawingElementType::Line:
            EditLine(static_cast<LineElement<float>&>(*highLighted));
            break;
    }
}

void MainVm::PlaceNewNodeElement(const QPoint& pointerPosition)
{
    SceneElement<float>& scene = project->SceneRef();
    scene.Highlight(scene.end());

    std::unique_ptr<DrawingElement<float>> newNode = NodeElement<float>::UniqueElement(SharedNode(pointerPosition));
    newNode->SetHighlighted(true);
    scene.Add(std::move(newNode));
}

void MainVm::NewNodeElement(const QPoint& mousePos, const QSize& labelSize)
{
    if (!surface)
        return;

    CancelOperation();

    QPoint translated = Geometry::TranslatePoint(mousePos, labelSize, surface->Size(), true);

    PlaceNewNodeElement(translated);

    mouseOperationState = MouseOperationStatus::NewNode;
    emit MouseOperationStateChanged(mouseOperationState);
}

void MainVm::NewLineElement(const QPoint&, const QSize&)
{
    if (!surface)
        return;

    CancelOperation();

    mouseOperationState = MouseOperationStatus::NewLineP1;
    emit MouseOperationStateChanged(mouseOperationState);
}

void MainVm::StartSimulation()
{
    if (started)
        return;

    emit RunSimulatorWorker();

    runTimer.restart();

    started = true;
}

void MainVm::StopSimulation()
{
    if (!started)
        return;

    emit CancelSimulatorWorker();

    started = false;
}

void MainVm::InitNewProject(std::unique_ptr<Project>&& newProject)
{
    if (simulatorWorker)
        simulatorWorker->deleteLater();

    simulatorWorker = new SimulatorWorker(newProject->SharedSimulator());

#ifdef _OPENMP
    simulatorWorker->SetNumThreads(numThreads);
#endif

    connect(&simulatorThread, &QThread::finished, simulatorWorker, &SimulatorWorker::deleteLater);
    connect(this, &MainVm::RunSimulatorWorker, simulatorWorker, &SimulatorWorker::Run);
    connect(this, &MainVm::CancelSimulatorWorker, simulatorWorker, &SimulatorWorker::Cancel, Qt::DirectConnection);

    simulatorWorker->moveToThread(&simulatorThread);

    project = std::move(newProject);
}

#ifdef QT_DEBUG
void MainVm::CreateScene()
{
    SceneElement<float>& scene = project->SceneRef();

    SharedNode anodeLeft(100, 500);
    SharedNode anodeRight(500, 500);

    SharedNode cathodeLeft(100, 100);
    SharedNode cathodeRight(500, 100);

    scene.Add(NodeElement<float>::UniqueElement(anodeRight));
    scene.Add(NodeElement<float>::UniqueElement(anodeLeft));
    scene.Add(NodeElement<float>::UniqueElement(cathodeLeft));
    scene.Add(NodeElement<float>::UniqueElement(cathodeRight));

    scene.Add(LineElement<float>::UniqueElement(anodeLeft, anodeRight, 1));
    scene.Add(LineElement<float>::UniqueElement(cathodeLeft, cathodeRight, -1));
}
#endif

void MainVm::CreateBorder(float voltage)
{
    SceneElement<float>& scene = project->SceneRef();
    QSize size = project->SharedSimulator()->Size();

    SharedNode topLeft(0, size.width() - 1);
    SharedNode topRight(size.height() - 1, size.width() - 1);
    SharedNode bottomLeft(0, 0);
    SharedNode bottomRight(size.height(), 0);

    scene.Add(NodeElement<float>::UniqueElement(topLeft));
    scene.Add(NodeElement<float>::UniqueElement(topRight));
    scene.Add(NodeElement<float>::UniqueElement(bottomLeft));
    scene.Add(NodeElement<float>::UniqueElement(bottomRight));

    scene.Add(LineElement<float>::UniqueElement(topLeft, topRight, voltage));
    scene.Add(LineElement<float>::UniqueElement(bottomLeft, bottomRight, voltage));
    scene.Add(LineElement<float>::UniqueElement(topLeft, bottomLeft, voltage));
    scene.Add(LineElement<float>::UniqueElement(topRight, bottomRight, voltage));
}


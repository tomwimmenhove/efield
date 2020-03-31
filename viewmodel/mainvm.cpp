#include <cmath>

#include <QInputDialog>

#include "mainvm.h"
#include "pointinputdialog.h"

#include "model/floatsurfacedrawer.h"
#include "visualizer/visualizer.h"

MainVm::MainVm(QWidget* parent)
    : QObject(parent), parentWidget(parent)
{
    CreateScene();

    auto setFixedValuesFunc = std::bind(&MainVm::SetFixedValues, this, std::placeholders::_1);
    simulator = QSharedPointer<Simulator>(new Simulator(601, 601, setFixedValuesFunc));

    simulatorWorker = new SimulatorWorker(simulator);

#ifdef _OPENMP
    simulatorWorker->SetNumThreads(6);
#endif

    connect(&simulatorThread, &QThread::finished, simulatorWorker, &SimulatorWorker::deleteLater);
    connect(this, &MainVm::RunSimulatorWorker, simulatorWorker, &SimulatorWorker::Run);
    connect(this, &MainVm::CancelSimulatorWorker, simulatorWorker, &SimulatorWorker::Cancel, Qt::DirectConnection);

    simulatorWorker->moveToThread(&simulatorThread);
    simulatorThread.start();
}

MainVm::~MainVm()
{
    emit CancelSimulatorWorker();
    simulatorThread.quit();
    simulatorThread.wait();
}

void MainVm::UpdateVisualization(bool useGradiant)
{
    if (useGradiant)
    {
        gradient = QSharedPointer<GradientSurface>(new GradientSurface(*simulator->CloneSurface()));
        surface = QSharedPointer<FloatSurface>(new FloatSurface(*gradient));
    }
    else
    {
        surface = simulator->CloneSurface();
        gradient = nullptr;
    }

    emit VisualizationAvailable(surface->MinValue(), surface->MaxValue());
}

void MainVm::RequestVisualization(const SimpleValueStepper& stepper, const QSize& size)
{
    QPixmap pixmapObject = QPixmap::fromImage(Visualizer::QImageFromFloatSurface(*surface, stepper));
    QPixmap scaledPixmap = pixmapObject.scaled(size, Qt::KeepAspectRatio);//, Qt::SmoothTransformation);

//    Test if the normal queued connection fucks shit up? This happened OUTSIDE of simulation
//    Check if this function is even re-entered. I don't think it can...
//    Also, size of 1104x546? Seems high? Maybe not...

    QPainter painter(&scaledPixmap);

    if (gradient)
    {
        painter.setPen(Qt::black);
        //painter.setRenderHint(QPainter::Antialiasing);

        Visualizer::PaintGradientVectors(painter, *gradient, 15);
    }

    painter.setRenderHint(QPainter::Antialiasing);
    scene.DrawAnnotation(painter, surface->Size());

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
    QSharedPointer<DrawingElement<float>> closest = scene.ClosestElement(pointerPosition);
    QSharedPointer<DrawingElement<float>> highLighted = scene.FindHighLigted();

    MouseOperationStatus oldState = mouseOperationState;

    switch(mouseOperationState)
    {
        case MouseOperationStatus::Normal:
            if (closest && closest == highLighted && highLighted->ElementType() == DrawingElementType::Node)
            {
                mouseOperationState = MouseOperationStatus::DragNode;
                QSharedPointer<NodeElement<float>> node = highLighted.staticCast<NodeElement<float>>();
                nodeSavedPos = node->Node();
                break;
            }

            scene.Highlight(closest);
            break;
        case MouseOperationStatus::DragNode:
            scene.Highlight(nullptr);
            mouseOperationState = MouseOperationStatus::Normal;
            break;
        case MouseOperationStatus::NewNode:
            PlaceNewNodeElement(pointerPosition);
            break;
        case MouseOperationStatus::NewLineP1:
            if (closest && highLighted)
            {
                QSharedPointer<NodeElement<float>> startNode = highLighted.staticCast<NodeElement<float>>();
                SharedNode sharedStartNode = startNode->Node();
                SharedNode sharedEndNode = SharedNode(pointerPosition);
                QSharedPointer<LineElement<float>> newLine = LineElement<float>::SharedElement(sharedStartNode, sharedEndNode, 0);

                scene.Add(newLine);

                /* Weak reference, so we don't have to clean up after it. Just let it hang out. */
                NewLine = newLine.toWeakRef();

                mouseOperationState = MouseOperationStatus::NewLineP2;
                break;
            }
            return;
        case MouseOperationStatus::NewLineP2:
            if (highLighted)
            {
                /* Set the definitive end point for the new line segment */
                QSharedPointer<NodeElement<float>> endNode = highLighted.staticCast<NodeElement<float>>();
                Q_ASSERT(NewLine);
                NewLine.toStrongRef()->SetP2(endNode->Node());

                mouseOperationState = MouseOperationStatus::NewLineP1;
            }
            scene.Highlight(nullptr);
            break;
    }

    if (oldState != mouseOperationState)
        emit MouseOperationStateChanged(mouseOperationState);

    emit VisualizationAvailable(surface->MinValue(), surface->MaxValue());
}

void MainVm::CancelOperation()
{
    QSharedPointer<DrawingElement<float>> highLighted = scene.FindHighLigted();

    MouseOperationStatus oldState = mouseOperationState;

    switch(mouseOperationState)
    {
        case MouseOperationStatus::Normal:
            return;
        case MouseOperationStatus::DragNode:
        {
            QSharedPointer<NodeElement<float>> node = highLighted.staticCast<NodeElement<float>>();
            node->Node().SetPosition(nodeSavedPos);
            mouseOperationState = MouseOperationStatus::Normal;
            break;
        }
        case MouseOperationStatus::NewNode:
            scene.Highlight(nullptr);
            scene.Remove(highLighted);
            mouseOperationState = MouseOperationStatus::Normal;
            break;
        case MouseOperationStatus::NewLineP1:
            scene.Highlight(nullptr);
            mouseOperationState = MouseOperationStatus::Normal;
            break;
        case MouseOperationStatus::NewLineP2:
            scene.Remove(NewLine);
            mouseOperationState = MouseOperationStatus::Normal;
            scene.Highlight(nullptr);
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

    QSharedPointer<DrawingElement<float>> closestNode = scene.ClosestElement(translated, 15, DrawingElementType::Node);
    QSharedPointer<DrawingElement<float>> highLighted = scene.FindHighLigted();

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
            QSharedPointer<NodeElement<float>> node = highLighted.staticCast<NodeElement<float>>();
            node->Node().SetPosition(translated);
            break;
        }
        case MouseOperationStatus::NewLineP1:
            /* Highlight close nodes */
            if (!closestNode)
                scene.Highlight(nullptr);
            else if (closestNode->ElementType() == DrawingElementType::Node)
                scene.Highlight(closestNode);
            break;
        case MouseOperationStatus::NewLineP2:
            if (!closestNode)
                scene.Highlight(nullptr);
            else if (closestNode->ElementType() == DrawingElementType::Node)
                scene.Highlight(closestNode);

            Q_ASSERT(NewLine);
            NewLine.toStrongRef()->P2().SetPosition(translated);
            break;
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

    QSharedPointer<DrawingElement<float>> closest = scene.ClosestElement(translated);
    if (!closest)
        return;

    switch (closest->ElementType())
    {
        case DrawingElementType::None:
        case DrawingElementType::Scene:
            break;
        case DrawingElementType::Node:
            EditNode(closest.staticCast<NodeElement<float>>());
            break;
        case DrawingElementType::Line:
            EditLine(closest.staticCast<LineElement<float>>());
            break;
    }
}

void MainVm::DeleteSelectedElement()
{
    if (!surface)
        return;

    bool isModified = false;

    QSharedPointer<DrawingElement<float>> highLighted = scene.FindHighLigted();
    if (!highLighted)
        return;

    if (highLighted->ElementType() == DrawingElementType::Node)
    {
        QSharedPointer<NodeElement<float>> node = highLighted.staticCast<NodeElement<float>>();

        // Can't delete nodes that are used by other elements */
        if (node->Node()->RefCount() > 0)
            return;
    }

    if (highLighted)
        isModified |= scene.Remove(highLighted) != 0;

    if (isModified)
        emit VisualizationAvailable(surface->MinValue(), surface->MaxValue());
}

void MainVm::EditNode(QSharedPointer<NodeElement<float>> node)
{
    SharedNode sharedNode = node->Node();

    PointInputDialog d(sharedNode, QPoint(0, 0), QPoint(surface->Width() - 1, surface->Height() - 1), parentWidget);
    if (d.exec() != QDialog::Accepted)
        return;

    sharedNode.SetPosition(d.Point());

    emit VisualizationAvailable(surface->MinValue(), surface->MaxValue());
}

void MainVm::EditLine(QSharedPointer<LineElement<float>> line)
{
    bool ok;
    int def = line->Value();

    int volt = QInputDialog::getInt(parentWidget, tr("Edit line"),
                                    tr("Voltage: "),  def, -2147483647, 2147483647, 1, &ok);
    if (ok)
        line->SetValue(volt);
}

void MainVm::EditSelectedElement()
{
    if (!surface || mouseOperationState != MouseOperationStatus::Normal)
        return;

    QSharedPointer<DrawingElement<float>> highLighted = scene.FindHighLigted();
    if (!highLighted)
        return;

    switch (highLighted->ElementType())
    {
        case DrawingElementType::None:
        case DrawingElementType::Scene:
            break;
        case DrawingElementType::Node:
            EditNode(highLighted.staticCast<NodeElement<float>>());
            break;
        case DrawingElementType::Line:
            EditLine(highLighted.staticCast<LineElement<float>>());
            break;
    }
}

void MainVm::PlaceNewNodeElement(const QPoint& pointerPosition)
{
    QSharedPointer<DrawingElement<float>> newNode = NodeElement<float>::SharedElement(SharedNode(pointerPosition));
    scene.Add(newNode);
    scene.Highlight(newNode);
}

void MainVm::NewNodeElement(const QPoint& mousePos, const QSize& labelSize)
{
    if (!surface || mouseOperationState != MouseOperationStatus::Normal)
        return;

    QPoint translated = Geometry::TranslatePoint(mousePos, labelSize, surface->Size(), true);

    PlaceNewNodeElement(translated);

    mouseOperationState = MouseOperationStatus::NewNode;
    emit MouseOperationStateChanged(mouseOperationState);
}

void MainVm::NewLineElement(const QPoint&, const QSize&)
{
    if (!surface || mouseOperationState != MouseOperationStatus::Normal)
        return;

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

void MainVm::CreateScene()
{
    SharedNode topLeft(0, 600);
    SharedNode topRight(600, 600);
    SharedNode bottomLeft(0, 0);
    SharedNode bottomRight(600, 0);

    SharedNode anodeLeft(100, 500);
    SharedNode anodeRight(500, 500);

    SharedNode cathodeLeft(100, 100);
    SharedNode cathodeRight(500, 100);

    scene.Add(NodeElement<float>::SharedElement(anodeRight));
    scene.Add(NodeElement<float>::SharedElement(anodeLeft));
    scene.Add(NodeElement<float>::SharedElement(cathodeLeft));
    scene.Add(NodeElement<float>::SharedElement(cathodeRight));

    scene.Add(LineElement<float>::SharedElement(topLeft, topRight, 0));
    scene.Add(LineElement<float>::SharedElement(bottomLeft, bottomRight, 0));
    scene.Add(LineElement<float>::SharedElement(topLeft, bottomLeft, 0));
    scene.Add(LineElement<float>::SharedElement(topRight, bottomRight, 0));

    scene.Add(LineElement<float>::SharedElement(anodeLeft, anodeRight, 1));
    scene.Add(LineElement<float>::SharedElement(cathodeLeft, cathodeRight, -1));
}

void MainVm::SetFixedValues(FloatSurface& surface)
{
    FloatSurfaceDrawer drawer(surface);

    scene.Draw(drawer);
}


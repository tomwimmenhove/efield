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

void MainVm::MousePressedOnPixmap(QPoint mousePos, Qt::MouseButtons buttons, QSize labelSize)
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
    QSharedPointer<DrawingElement<float>> highLighted = scene.FindHighLigted();

    bool doUpdate = false;
    switch(mouseMoveStatus)
    {
        case MouseMoveStatus::Normal:
            if (buttons != Qt::LeftButton)
                break;

            if (closest && closest == highLighted && highLighted->ElementType() == DrawingElementType::Node)
            {
                mouseMoveStatus = MouseMoveStatus::DragNode;
                QSharedPointer<NodeElement<float>> node = highLighted.staticCast<NodeElement<float>>();
                nodeSavedPos = node->Node();
                break;
            }

            scene.Highlight(closest);

            doUpdate = true;

            break;
        case MouseMoveStatus::DragNode:
            if (buttons == Qt::LeftButton)
                scene.Highlight(nullptr);

            if (buttons == (Qt::RightButton | Qt::LeftButton))
            {
                QSharedPointer<NodeElement<float>> node = highLighted.staticCast<NodeElement<float>>();
                node->Node().SetPoint(nodeSavedPos);
            }

            doUpdate = true;

            mouseMoveStatus = MouseMoveStatus::Normal;
            break;
        case MouseMoveStatus::NewNode:
            scene.Highlight(nullptr);
            if (buttons != Qt::LeftButton)
                scene.Remove(highLighted);

            doUpdate = true;

            mouseMoveStatus = MouseMoveStatus::Normal;
            break;
        case MouseMoveStatus::NewLineP1:
            if (buttons == Qt::LeftButton && closest && highLighted)
            {
                QSharedPointer<NodeElement<float>> startNode = highLighted.staticCast<NodeElement<float>>();
                SharedNode sharedStartNode = startNode->Node();
                SharedNode sharedEndNode = SharedNode(translated);
                NewLine = LineElement<float>::SharedElement(sharedStartNode, sharedEndNode, 0);

                scene.Add(NewLine);

                mouseMoveStatus = MouseMoveStatus::NewLineP2;
            }
            if (buttons == Qt::RightButton)
            {
                scene.Highlight(nullptr);
                mouseMoveStatus = MouseMoveStatus::Normal;
            }
            doUpdate = true;
            break;
        case MouseMoveStatus::NewLineP2:
            if (buttons == Qt::LeftButton && highLighted)
            {
                /* Set the definitive end point for the new line segment */
                QSharedPointer<NodeElement<float>> endNode = highLighted.staticCast<NodeElement<float>>();
                NewLine->SetP2(endNode->Node());

                mouseMoveStatus = MouseMoveStatus::Normal;
            }

            if (buttons == Qt::RightButton)
            {
                scene.Remove(NewLine);
                mouseMoveStatus = MouseMoveStatus::Normal;
            }

            scene.Highlight(nullptr);

            doUpdate = true;

            break;
    }

    if (doUpdate)
        emit VisualizationAvailable(surface->MinValue(), surface->MaxValue());
}

void MainVm::MouseMovedOnPixmap(QPoint mousePos, QSize labelSize)
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
    QSharedPointer<DrawingElement<float>> highLighted = scene.FindHighLigted();

    bool doUpdate = false;
    switch(mouseMoveStatus)
    {
        case MouseMoveStatus::Normal:
        {
            if (gradient)
            {
                QVector2D v = gradient->XYValue(translated.x(), translated.y());

                emit NewStatusMessage(QString(tr("Value at [%1, %2]: %3 @%4° (Vector [%5, %6])"))
                                      .arg(translated.x())
                                      .arg(translated.y())
                                      .arg(v.length())
                                      .arg(atan2(v.y(), v.x()) * 180 / M_PI)
                                      .arg(v.x())
                                      .arg(v.y()));
            }
            else
            {
                float value = surface->XYValue(translated.x(), translated.y());
                emit NewStatusMessage(QString(tr("Value at [%1, %2]: %3")).arg(translated.x()).arg(translated.y()).arg(value));
            }
            break;
        }
        case MouseMoveStatus::DragNode:
        case MouseMoveStatus::NewNode:
        {
            QSharedPointer<NodeElement<float>> node = highLighted.staticCast<NodeElement<float>>();
            node->Node().SetPoint(translated);

            doUpdate = true;
            break;
        }
        case MouseMoveStatus::NewLineP1:
            /* Highlight close nodes */
            if (!closest)
                scene.Highlight(nullptr);
            else if (closest->ElementType() == DrawingElementType::Node)
                scene.Highlight(closest);

            doUpdate = true;
            break;
        case MouseMoveStatus::NewLineP2:
            if (!closest)
                scene.Highlight(nullptr);
            else if (closest->ElementType() == DrawingElementType::Node)
                scene.Highlight(closest);

            NewLine->P2().SetPoint(translated);

            doUpdate = true;
            break;
    }

    if (doUpdate)
        emit VisualizationAvailable(surface->MinValue(), surface->MaxValue());
}

void MainVm::MouseReleasedFromPixmap(QPoint mousePos, Qt::MouseButtons buttons, QSize labelSize)
{
    if (buttons == Qt::RightButton)
        return;

    switch(mouseMoveStatus)
    {
        case MouseMoveStatus::Normal:
            break;
        case MouseMoveStatus::DragNode:
            mouseMoveStatus = MouseMoveStatus::Normal;
            break;
        case MouseMoveStatus::NewNode:
            break;
        case MouseMoveStatus::NewLineP1:
            break;
        case MouseMoveStatus::NewLineP2:
            break;
    }
}

void MainVm::MouseDoubleClickedOnPixmap(QPoint mousePos, Qt::MouseButtons buttons, QSize labelSize)
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
        QSharedPointer<NodeElement<float>> node =highLighted.staticCast<NodeElement<float>>();

        // Can't delete nodes that are used by other elements */
        if (node->Node()->RefCount() > 0)
            return;
    }

    if (highLighted)
        isModified |= scene.Remove(highLighted) != 0;

    if (isModified)
    {
        SetFixedValues(*surface);

        emit VisualizationAvailable(surface->MinValue(), surface->MaxValue());
    }
}

void MainVm::EditNode(QSharedPointer<NodeElement<float>> node)
{
    SharedNode sharedNode = node->Node();

    PointInputDialog d(sharedNode, QPoint(0, 0), QPoint(surface->Width() - 1, surface->Height() - 1), parentWidget);
    if (d.exec() != QDialog::Accepted)
        return;

    sharedNode.SetPoint(d.Point());

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
    if (!surface || mouseMoveStatus != MouseMoveStatus::Normal)
        return;

    QSharedPointer<DrawingElement<float>> highLighted = scene.FindHighLigted();
    if (!highLighted)
        return;

    switch (highLighted->ElementType())
    {
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

void MainVm::NewNodeElement(const QPoint& mousePos, const QSize& labelSize)
{
    if (!surface || mouseMoveStatus != MouseMoveStatus::Normal)
        return;

    QPoint translated = Geometry::TranslatePoint(mousePos, labelSize, surface->Size(), true);

    QSharedPointer<DrawingElement<float>> newNode = NodeElement<float>::SharedElement(SharedNode(translated));
    scene.Add(newNode);
    scene.Highlight(newNode);

    mouseMoveStatus = MouseMoveStatus::NewNode;
}

void MainVm::NewLineElement(const QPoint& mousePos, const QSize& labelSize)
{
    if (!surface || mouseMoveStatus != MouseMoveStatus::Normal)
        return;

    mouseMoveStatus = MouseMoveStatus::NewLineP1;
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

    //anodeLeft = QPoint(0, 0);
}

void MainVm::SetFixedValues(FloatSurface& surface)
{
    FloatSurfaceDrawer drawer(surface);

    scene.Draw(drawer);
}


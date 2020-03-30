#include <cmath>

#include "mainvm.h"
#include "pointinputdialog.h"

#include "model/floatsurfacedrawer.h"
#include "visualizer/visualizer.h"
#include "graphics/lineelement.h"
#include "graphics/nodeelement.h"

MainVm::MainVm()
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

    if (dragginNode)
    {
        NodeElement<float>* node = static_cast<NodeElement<float>*>(dragginNode.data());

        SharedNode sharedNode = node->Node();

        sharedNode = translated;

        emit VisualizationAvailable(surface->MinValue(), surface->MaxValue());
    }

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
}

void MainVm::MousePressedOnPixmap(QPoint mousePos, Qt::MouseButtons buttons, QSize labelSize)
{
    if (!surface)
        return;

    if (buttons == Qt::RightButton)
    {
        scene.Highlight(nullptr);
        emit VisualizationAvailable(surface->MinValue(), surface->MaxValue());

        return;
    }

    QPoint translated = Geometry::TranslatePoint(mousePos, labelSize, surface->Size(), true);
    QSharedPointer<DrawingElement<float>> closest = scene.ClosestElement(translated, 10);

    QSharedPointer<DrawingElement<float>> highLighted = scene.FindHighLigted();
    if (closest == highLighted)
    {
        NodeElement<float>* node = dynamic_cast<NodeElement<float>*>(highLighted.data());
        if (node)
        {
            dragginNode = highLighted;
            return;
        }
    }

//    QSharedPointer<DrawingElement<T>> closest = scene.ClosestElement(translated, 10);
    scene.Highlight(closest);
//    scene.HighlightClosestElement(translated, 10);

    emit VisualizationAvailable(surface->MinValue(), surface->MaxValue());
}

void MainVm::MouseReleasedFromPixmap(QPoint mousePos, Qt::MouseButtons buttons, QSize labelSize)
{
    dragginNode = nullptr;
}

void MainVm::MouseDoubleClickedOnPixmap(QPoint mousePos, Qt::MouseButtons buttons, QSize labelSize)
{
    if (!surface)
        return;

    QPoint translated = Geometry::TranslatePoint(mousePos, labelSize, surface->Size(), true);
    QSharedPointer<DrawingElement<float>> closest = scene.ClosestElement(translated, 10);

    NodeElement<float>* node = dynamic_cast<NodeElement<float>*>(closest.data());
    if (!node)
        return;

    SharedNode sharedNode = node->Node();

    PointInputDialog d(sharedNode, QPoint(0, 0), QPoint(surface->Width() - 1, surface->Height() - 1));
    if (d.exec() != QDialog::Accepted)
        return;

    sharedNode = d.Point();

    emit VisualizationAvailable(surface->MinValue(), surface->MaxValue());
}

void MainVm::DeleteSelectedElement()
{
    if (!surface)
        return;

    bool isModified = false;

    QSharedPointer<DrawingElement<float>> highLighted = scene.FindHighLigted();

    // Fuck off, this is a perfectly good use for dynamic casting
    DrawingElement<float>* elemp = highLighted.data();
    NodeElement<float>* node = dynamic_cast<NodeElement<float>*>(elemp);

    // Can't delete nodes that are used by other elements */
    if (node && node->Node()->RefCount() > 0)
        return;

    if (highLighted)
        isModified |= scene.Remove(highLighted) != 0;

    if (isModified)
    {
        SetFixedValues(*surface);

        emit VisualizationAvailable(surface->MinValue(), surface->MaxValue());
    }
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

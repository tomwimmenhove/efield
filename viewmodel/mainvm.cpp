#include <cmath>

#include "mainvm.h"

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

    QPoint translated = Geometry::TranslatePoint(mousePos, labelSize, surface->Size(), true);

    scene.HighLightClosestElement(translated, 10);

    emit VisualizationAvailable(surface->MinValue(), surface->MaxValue());
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

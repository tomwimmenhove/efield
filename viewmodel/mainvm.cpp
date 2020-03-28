#include <cmath>

#include "mainvm.h"

#include "model/floatsurfacedrawer.h"
#include "visualizer/visualizer.h"
#include "graphics/lineelement.h"

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

    emit UpdateDone(surface->MinValue(), surface->MaxValue());
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
    scene.DrawAnnotation(painter, scaledPixmap.size(), surface->Size());

    frames++;
    if (frames % 25 == 0)
    {
        qDebug() << ((float) simulatorWorker->Iterations() * 1000.0 / runTimer.elapsed()) << " iterations/sec";
    }

    emit NewVisualization(scaledPixmap);
}

void MainVm::MouseMovedOnPixmap(QPoint mousePos, QSize pixmapSize)
{
    if (!surface)
        return;

    float scale = qMin((float) pixmapSize.width()  / surface->Width(),
                       (float) pixmapSize.height() / surface->Height());

    // Should we round these?
    float scaledWidth = scale * surface->Width();
    float scaledHeight = scale * surface->Height();

    int xStart = (pixmapSize.width() - scaledWidth) / 2;
    int yStart = (pixmapSize.height() - scaledHeight) / 2;

    int valueX = (mousePos.x() - xStart) * surface->Width() / scaledWidth;
    int valueY = (scaledHeight - (mousePos.y() - yStart) - 1) * surface->Height() / scaledHeight;

    if (valueX < 0 || valueX >= surface->Width() || valueY < 0 || valueY >= surface->Height())
    {
        emit NewStatusMessage(QString(tr("[%1, %2]")).arg(valueX).arg(valueY));
        return;
    }

    if (gradient)
    {
        QVector2D v = gradient->XYValue(valueX, valueY);

        emit NewStatusMessage(QString(tr("Value at [%1, %2]: %3 @%4° (Vector [%5, %6])"))
                                 .arg(valueX)
                                 .arg(valueY)
                                 .arg(v.length())
                                 .arg(atan2(v.y(), v.x()) * 180 / M_PI)
                                 .arg(v.x())
                                 .arg(v.y()));
    }
    else
    {
        float value = surface->XYValue(valueX, valueY);
        emit NewStatusMessage(QString(tr("Value at [%1, %2]: %3")).arg(valueX).arg(valueY).arg(value));
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

    DrawingElement<float>* top = new LineElement<float>(topLeft, topRight, 0);
    DrawingElement<float>* bottom = new LineElement<float>(bottomLeft, bottomRight, 0);
    DrawingElement<float>* left = new LineElement<float>(topLeft, bottomLeft, 0);
    DrawingElement<float>* right = new LineElement<float>(topRight, bottomRight, 0);

    DrawingElement<float>* anode = new LineElement<float>(anodeLeft, anodeRight, 1);
    DrawingElement<float>* cathode = new LineElement<float>(cathodeLeft, cathodeRight, -1);

    scene.Add(top);
    scene.Add(bottom);
    scene.Add(left);
    scene.Add(right);

    scene.Add(anode);
    scene.Add(cathode);

    //anodeLeft = QPoint(0, 0);
}

void MainVm::SetFixedValues(FloatSurface& surface)
{
    FloatSurfaceDrawer drawer(surface);

    scene.Draw(drawer);
}

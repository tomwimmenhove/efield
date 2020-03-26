#include <cmath>

#include "mainvm.h"

#include "model/floatsurfacedrawer.h"
#include "visualizer/visualizer.h"

MainVm::MainVm()
{
    simulator = QSharedPointer<Simulator>(new Simulator(601, 601, SetFixedValues));
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

void MainVm::RequestVisualization(const SimpleValueStepper& stepper, int width, int height)
{
    QPixmap pixmapObject = QPixmap::fromImage(Visualizer::QImageFromFloatSurface(*surface, stepper));
    QPixmap scaledPixmap = pixmapObject.scaled(width, height, Qt::KeepAspectRatio);//, Qt::SmoothTransformation);

    if (gradient)
    {
        QPainter painter(&scaledPixmap);

        painter.setPen(Qt::black);
        //painter.setRenderHint(QPainter::Antialiasing);

        Visualizer::PaintGradientVectors(painter, *gradient, 15);
    }

    if (simulatorThread)
    {
        frames++;
        if (frames % 25 == 0)
        {
            qDebug() << ((float) simulatorThread->Iterations() * 1000.0 / runTimer.elapsed()) << " iterations/sec";
        }
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

    simulatorThread = new SimulatorThread(simulator);
#ifdef _OPENMP
    simulatorThread->SetNumThreads(6);
#endif

    connect(simulatorThread, &SimulatorThread::finished, simulatorThread, &QObject::deleteLater);

    simulatorThread->start();

    runTimer.restart();

    started = true;
}

void MainVm::StopSimulation()
{
    if (!started)
        return;

    simulatorThread->Cancel();
    simulatorThread = nullptr;

    started = false;
}

void MainVm::SetFixedValues(FloatSurface& surface)
{
    FloatSurfaceDrawer drawer(surface);
    Drawing<float> drawing(drawer);

    int n = 1;

    drawing.DrawLine(0/n, 0/n, 600/n, 0/n, 0);
    drawing.DrawLine(0/n, 600/n, 600/n, 600/n, 0);

    drawing.DrawLine(0/n, 0/n, 0/n, 600/n, 0);
    drawing.DrawLine(600/n, 0/n, 600/n, 600/n, 0);

    /* anode */
    drawing.DrawLine(100/n, 500/n, 500/n, 500/n, 1);
    //drawing.DrawLine(500/n, 500/n, 500/n, 300/n, 1);

    /* cathode */
    drawing.DrawLine(100/n, 100/n, 500/n, 100/n, -1);
}

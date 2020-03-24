#include "mainwindow.h"
#include "heatmap.h"
#include "ui_mainwindow.h"
#include "model/drawing.h"
#include "model/floatsurfacedrawer.h"
#include "model/floatsurface.h"
#include "model/gradientsurface.h"

#include <QPainter>
#include <QGraphicsScene>
#include <QDebug>
#include <QString>
#include <QColor>
#include <QtGlobal>
#include <math.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->graphicsLabel, &MouseEventLabel::Mouse_Moved, this, &MainWindow::GraphMouse_Moved);

    simulator = QSharedPointer<Simulator>(new Simulator(601, 601, SetFixedValues));

    arrow = MakeArrow();

    frameTimer = new QTimer(this);
    connect(frameTimer, &QTimer::timeout, this, &MainWindow::FrameUpdate);

    statusBar()->show();
    //StartSimulation();
    //FrameUpdate();
}

QPixmap MainWindow::MakeArrow()
{
    QPixmap arrow = QPixmap(9, 9);
    arrow.fill(QColor(0, 0, 0, 255));
    arrow.fill(QColor(0, 0, 0, 0));
    QPainter arrowPainter(&arrow);
    //arrowPainter.setRenderHint(QPainter::Antialiasing);
    arrowPainter.drawLine(0, 4, 8, 4);
    arrowPainter.drawLine(4, 0, 8, 4);
    arrowPainter.drawLine(4, 8, 8, 4);

    return arrow;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetFixedValues(FloatSurface& surface)
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

void MainWindow::GraphMouse_Moved(int x, int y)
{
    if (!surface)
        return;

    float scale = qMin((float) ui->graphicsLabel->width() / surface->Width(), (float) ui->graphicsLabel->height() / surface->Height());

    // Should we round these?
    float scaledWidth = scale * surface->Width();
    float scaledHeight = scale * surface->Height();

    int xStart = (ui->graphicsLabel->width() - scaledWidth) / 2;
    int yStart = (ui->graphicsLabel->height() - scaledHeight) / 2;

    int valueX = (x - xStart) * surface->Width() / scaledWidth;
    int valueY = (scaledHeight - (y - yStart) - 1) * surface->Height() / scaledHeight;

    if (valueX < 0 || valueX >= surface->Width() || valueY < 0 || valueY >= surface->Height())
    {
        statusBar()->showMessage(QString(tr("[%1, %2]")).arg(valueX).arg(valueY));
        return;
    }

    statusBar()->show();
    if (gradient)
    {
        QVector2D v = gradient->XYValue(valueX, valueY);

        statusBar()->showMessage(QString(tr("Value at [%1, %2]: %3 @%4° (Vector [%5, %6])"))
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
        statusBar()->showMessage(QString(tr("Value at [%1, %2]: %3")).arg(valueX).arg(valueY).arg(value));
    }
}

void MainWindow::FrameUpdate()
{
    if (ui->actionGradient->isChecked())
    {
        gradient = QSharedPointer<GradientSurface>(new GradientSurface(*simulator->CloneSurface()));
        surface = QSharedPointer<FloatSurface>(new FloatSurface(*gradient));
    }
    else
    {
        surface = simulator->CloneSurface();
        gradient = nullptr;
    }

    int w = surface->Width();
    int h = surface->Height();
    float max = surface->MaxValue();
    float min = surface->MinValue();
    float range = max - min;

    ui->heatMapLegend->SetMin(min);
    ui->heatMapLegend->SetMax(max);

    std::vector<QRgb> pixels(w * h);
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            /* Scale between  0..1 */
            if (range != 0)
            {
                float f = ui->heatMapLegend->GetSteppedValue((surface->XYCValue(x, y)) - min) / range;

                pixels[x + (h - y - 1) * h] = HeatMap::GetColor(f, 0);
            }
        }
    }

    QImage image = QImage((uchar*)pixels.data(), w, h, QImage::Format_ARGB32);
    QPixmap pixmapObject = QPixmap::fromImage(image);
    QPixmap scaledPixmap = pixmapObject.scaled(ui->graphicsLabel->width(), ui->graphicsLabel->height(), Qt::KeepAspectRatio);//, Qt::SmoothTransformation);

    if (gradient)
    {
        QPainter painter(&scaledPixmap);
        painter.setPen(Qt::black);
        //painter.setRenderHint(QPainter::Antialiasing);

        for (int y = 0; y < scaledPixmap.height(); y += 15)
        {
            for (int x = 0; x < scaledPixmap.width(); x += 15)
            {
                int nx = x * w / scaledPixmap.width();
                int ny = y * h / scaledPixmap.height();

                QVector2D v = -gradient->XYCValue(nx, ny); // Negated, because E-field lines go from positive to negative
                float a = atan2(v.y(), v.x());
                QMatrix rm;
                rm.rotate(a * 180 / M_PI);

                painter.drawPixmap(x - arrow.width(), y - arrow.height(), arrow.transformed(rm));
            }
        }
    }

    ui->graphicsLabel->setPixmap(scaledPixmap);

    frames++;
    if (frames % 25 == 0)
    {
        qDebug() << ((float) simulatorThread->Iterations() * 1000.0 / runTimer.elapsed()) << " iterations/sec";
    }
}

void MainWindow::on_actionStart_triggered()
{
    StartSimulation();
}

void MainWindow::on_actionS_top_triggered()
{
    StopSimulation();
}

void MainWindow::StartSimulation()
{
    simulatorThread = new SimulatorThread(simulator);
#ifdef _OPENMP
    simulatorThread->SetNumThreads(6);
#endif

    connect(simulatorThread, &SimulatorThread::finished, simulatorThread, &QObject::deleteLater);

    simulatorThread->start();
    frameTimer->start(40);
    runTimer.restart();
}

void MainWindow::StopSimulation()
{
    frameTimer->stop();
    simulatorThread->Cancel();
}

void MainWindow::on_actionSave_Image_triggered()
{
    if (ui->actionGradient->isChecked())
    {
        ui->graphicsLabel->pixmap()->save("/tmp/efield_gradient.png");
    }
    else
    {
        ui->graphicsLabel->pixmap()->save("/tmp/efield_potential.png");
    }
}

void MainWindow::on_actionGradient_triggered()
{
    FrameUpdate();
}

void MainWindow::on_actionStepped_triggered()
{
    ui->heatMapLegend->setStepped(ui->actionStepped->isChecked());

    FrameUpdate();
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
   QMainWindow::resizeEvent(event);

   FrameUpdate();
}

void MainWindow::on_actionRedraw_triggered()
{
    FrameUpdate();
}

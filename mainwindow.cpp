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
#include <math.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->graphicsLabel, &QClickableLabel::Mouse_Moved, this, &MainWindow::GraphMouse_Moved);
    connect(ui->graphicsLabel, &QClickableLabel::Mouse_Pressed, this, &MainWindow::GraphMouse_Pressed);
    connect(ui->graphicsLabel, &QClickableLabel::Mouse_Left, this, &MainWindow::GraphMouse_Left);

    simulator = QSharedPointer<Simulator>(new Simulator(250, 250, SetFixedValues));

    frameTimer = new QTimer(this);
    connect(frameTimer, &QTimer::timeout, this, &MainWindow::FrameUpdate);

    simulatorThread = new SimulatorThread(simulator);
    connect(simulatorThread, &SimulatorThread::finished, simulatorThread, &QObject::deleteLater);

#ifdef _OPENMP
    simulatorThread->SetNumThreads(6);
#endif

    simulatorThread->start();
    frameTimer->start(40);
    runTimer.start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetFixedValues(FloatSurface& surface)
{
    FloatSurfaceDrawer drawer(surface);
    Drawing<float> drawing(drawer);

    int n = 4;

    drawing.DrawLine(200/n, 200/n, 800/n, 200/n, 0);
    drawing.DrawLine(200/n, 800/n, 800/n, 800/n, 0);

    drawing.DrawLine(200/n, 200/n, 200/n, 800/n, 0);
    drawing.DrawLine(800/n, 200/n, 800/n, 800/n, 0);


    /* anode */
    drawing.DrawLine(300/n, 700/n, 700/n, 700/n, 1);
    //drawing.DrawLine(700/n, 700/n, 700/n, 500/n, 1);

    /* cathode */
    drawing.DrawLine(300/n, 300/n, 700/n, 300/n, -1);

    return;
    for (int x = 420/4; x < 630/4; x++)
    {
        surface.XYValue(x, 380/4) = 1;
        surface.XYValue(x, 700/4) = -1;
    }

    //    for (int y = 380/4; y < 600/4; y++)
    //    {
    //        surface.XYValue(630/4, y) = 1;
    //    }
}

void MainWindow::GraphMouse_Moved(int x, int y)
{
    if (!surface)
        return;

    int valueX = x * surface->Width() / ui->graphicsLabel->width();
    int valueY = (ui->graphicsLabel->height() - y - 1) * surface->Height() / ui->graphicsLabel->height();

    statusBar()->show();
    if (gradient)
    {
        QVector2D v = gradient->XYValue(valueX, valueY);

        statusBar()->showMessage(QString(tr("Value at [%1, %2]: %3 @%4°"))
                                 .arg(valueX)
                                 .arg(valueY)
                                 .arg(v.length())
                                 .arg(atan2 (v.y(), v.x()) * 180 / M_PI));
    }
    else
    {
        float value = surface->XYValue(valueX, valueY);
        statusBar()->showMessage(QString(tr("Value at [%1, %2]: %3")).arg(valueX).arg(valueY).arg(value));
    }
}

void MainWindow::GraphMouse_Pressed(int, int)
{
}

void MainWindow::GraphMouse_Left()
{
    statusBar()->hide();
}

void MainWindow::FrameUpdate()
{
    QSharedPointer<FloatSurface> s = simulator->CloneSurface();
    if (ui->dbGradient->checkState() == Qt::Checked)
    {
        gradient = QSharedPointer<GradientSurface>(new GradientSurface(*s));
        surface = QSharedPointer<FloatSurface>(new FloatSurface(*gradient));
    }
    else
    {
        surface = s;
        gradient = nullptr;
    }

    int w = surface->Width();
    int h = surface->Height();
    float max = surface->MaxValue();
    float min = surface->MinValue();
    float range = max - min;

    std::vector<QRgb> pixels(w * h);
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            /* Scale between  0..1 */
            if (range != 0)
            {
                float f = (surface->XYCValue(x, y) - min) / range;

                pixels[x + (h - y - 1) * h] = HeatMap::GetColor(f);
            }
        }
    }

    QImage image = QImage((uchar*)pixels.data(), w, h, QImage::Format_ARGB32);

    QPixmap pixmapObject = QPixmap::fromImage(image);

    ui->graphicsLabel->setPixmap(pixmapObject.scaled(ui->graphicsLabel->width(), ui->graphicsLabel->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    frames++;
    if (frames % 25 == 0)
    {
        qDebug() << ((float) simulatorThread->Iterations() * 1000.0 / runTimer.elapsed()) << " iterations/sec";
    }
}

void MainWindow::on_actionStart_triggered()
{
    simulatorThread->start();
    frameTimer->start(40);
    runTimer.start();
}

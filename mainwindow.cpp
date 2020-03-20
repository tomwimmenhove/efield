#include "mainwindow.h"
#include "heatmap.h"
#include "ui_mainwindow.h"

#include <QPainter>
#include <QGraphicsScene>
#include <QDebug>
#include <QString>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->graphicsLabel, &QClickableLabel::Mouse_Moved, this, &MainWindow::GraphMouse_Moved);
    connect(ui->graphicsLabel, &QClickableLabel::Mouse_Pressed, this, &MainWindow::GraphMouse_Pressed);
    connect(ui->graphicsLabel, &QClickableLabel::Mouse_Left, this, &MainWindow::GraphMouse_Left);

    simulator = QSharedPointer<Simulator>(new Simulator(500, 500, SetFixedValues));

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

void MainWindow::SetFixedValues(Surface& surface)
{
    for (int x = 420/2; x < 630/2; x++)
    {
        surface.XYValue(x, 380/2) = 1;
        //XYValue(x, 52) = 0;

        surface.XYValue(x, 700/2) = -1;
    }

    for (int y = 380/2; y < 600/2; y++)
    {
        surface.XYValue(630/2, y) = 1;
    }


//    for (int y = 0; y < 48; y++)
//    {
//        XYValue(50, y) = 1;
//    }

//    for (int y = 53; y < 100; y++)
//    {
//        XYValue(50, y) = 0;
//    }


//    for (int y = 49; y < 55;y++)
//    {
//        XYValue(37, y) = 1;
//        XYValue(64, y) = 1;
//    }

//    for (int x = 40; x < 60;x++)
//    {
//        XYValue(x, 52) = 0;
//    }
}

void MainWindow::GraphMouse_Moved(int x, int y)
{
    int valueX = x * simulator->Width() / ui->graphicsLabel->width();
    int valueY = y * simulator->Height() / ui->graphicsLabel->height();

    float value = simulator->CurrentSurface().XYValue(valueX, valueY);

    statusBar()->show();
    statusBar()->showMessage(QString(tr("Value at [%1, %2]: %3")).arg(valueX).arg(valueY).arg(value));
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
    QSharedPointer<Surface> surface = simulator->CloneSurface();

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
            float f = (surface->XYValue(x, y) - min) / range;

            pixels[x + y * h] = HeatMap::GetColor(f);
        }
    }

    QImage image((uchar*)pixels.data(), w, h, QImage::Format_ARGB32);
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

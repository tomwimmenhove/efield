#include "mainwindow.h"
#include "heatmap.h"
#include "ui_mainwindow.h"

#include <QPainter>
#include <QGraphicsScene>
#include <QElapsedTimer>
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

    SetFixedValues();

    frameTimer = new QTimer(this);
    connect(frameTimer, &QTimer::timeout, this, &MainWindow::FrameUpdate);
    frameTimer->start(40);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetFixedValues()
{
    for (int x = 420/4; x < 630/4;x++)
    {
        simulator.CurrentSurface().XYValue(x, 380/4) = 1;
        //XYValue(x, 52) = 0;

        simulator.CurrentSurface().XYValue(x, 700/4) = 0;
    }

    for (int y = 380/4; y < 600/4; y++)
    {
        simulator.CurrentSurface().XYValue(620/4, y) = 1;
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

void MainWindow::on_pushButton_clicked()
{

}

void MainWindow::GraphMouse_Moved(int x, int y)
{
    int valueX = x * simulator.Width() / ui->graphicsLabel->width();
    int valueY = y * simulator.Height() / ui->graphicsLabel->height();

    float value = simulator.CurrentSurface().XYValue(valueX, valueY);

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
    int numIter = 100;

    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < numIter; i++)
    {
        simulator.IterateSimulation();
        SetFixedValues();
    }

    qDebug() << numIter << "iterations or IterateSimulation() took" << timer.elapsed() << "milliseconds";

    auto surface = &simulator.CurrentSurface();//.Clone();

    int w = surface->Width();
    int h = surface->Height();
    float max = surface->MaxValue();
    float min = surface->MinValue();
    float range = max - min;

    QRgb* pixels = new QRgb[w * h];
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            /* Scale between  0..1 */
            float f = (surface->XYValue(x, y) - min) / range;

            pixels[x + y * h] = HeatMap::GetColor(f);
        }
    }

    QImage image((uchar*)pixels, w, h, QImage::Format_ARGB32);

    QPixmap pixmapObject = QPixmap::fromImage(image);

    //ui->graphicsLabel->setPixmap(pixmapObject.scaled(ui->graphicsLabel->width(), ui->graphicsLabel->height(), Qt::IgnoreAspectRatio));
    ui->graphicsLabel->setPixmap(pixmapObject.scaled(ui->graphicsLabel->width(), ui->graphicsLabel->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    delete[] pixels;
}

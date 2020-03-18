#include <tgmath.h>

#include "mainwindow.h"
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
        simulator.XYValue(x, 380/4) = 1;
        //XYValue(x, 52) = 0;

        simulator.XYValue(x, 700/4) = 0;
    }

    for (int y = 380/4; y < 600/4; y++)
    {
        simulator.XYValue(620/4, y) = 1;
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

QRgb getHeatMapColor(float value)
{
    /* Turn into 20 'steps' */
    value *= 20;
    value = round(value);
    value /= 20;

    const int NUM_COLORS = 4;
    static float color[NUM_COLORS][3] = { {0,0,255}, {0,255,0}, {255,255,0}, {255,0,0} };
    // A static array of 4 colors:  (blue,   green,  yellow,  red) using {r,g,b} for each.

    int idx1;        // |-- Our desired color will be between these two indexes in "color".
    int idx2;        // |
    float fractBetween = 0;  // Fraction between "idx1" and "idx2" where our value is.

    if(value <= 0)      {  idx1 = idx2 = 0;            }    // accounts for an input <=0
    else if(value >= 1)  {  idx1 = idx2 = NUM_COLORS-1; }    // accounts for an input >=0
    else
    {
        value = value * (NUM_COLORS-1);        // Will multiply value by 3.
        idx1  = value;                         // Our desired color will be after this index.
        idx2  = idx1+1;                        // ... and before this index (inclusive).
        fractBetween = value - float(idx1);    // Distance between the two indexes (0-1).
    }

    float red   = (color[idx2][0] - color[idx1][0])*fractBetween + color[idx1][0];
    float green = (color[idx2][1] - color[idx1][1])*fractBetween + color[idx1][1];
    float blue  = (color[idx2][2] - color[idx1][2])*fractBetween + color[idx1][2];

    return 0xff000000 | ((uint8_t) red) << 16 | ((uint8_t) green) << 8 | ((uint8_t) blue);
}

void MainWindow::on_pushButton_clicked()
{

}

void MainWindow::GraphMouse_Moved(int x, int y)
{
    int valueX = x * simulator.Width() / ui->graphicsLabel->width();
    int valueY = y * simulator.Height() / ui->graphicsLabel->height();

    float value = simulator.XYValue(valueX, valueY);

    statusBar()->show();
    statusBar()->showMessage(QString(tr("Value at [%1, %2]: %3")).arg(valueX).arg(valueY).arg(value));
}

void MainWindow::GraphMouse_Pressed(int x, int y)
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

    int w = simulator.Width();
    int h = simulator.Height();
    float max = simulator.MaxValue();
    float min = simulator.MinValue();
    float range = max - min;

    QRgb* pixels = new QRgb[w * h];
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            /* Scale between  0..1 */
            float f = (simulator.XYValue(x, y) - min) / range;

            pixels[x + y * h] = getHeatMapColor(f);
        }
    }

    QImage image((uchar*)pixels, w, h, QImage::Format_ARGB32);

    QPixmap pixmapObject = QPixmap::fromImage(image);

    ui->graphicsLabel->setPixmap(pixmapObject.scaled(ui->graphicsLabel->width(), ui->graphicsLabel->height(), Qt::IgnoreAspectRatio)); //KeepAspectRatio));

    delete[] pixels;
}

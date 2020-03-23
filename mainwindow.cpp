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
#include <math.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->graphicsLabel, &QClickableLabel::Mouse_Moved, this, &MainWindow::GraphMouse_Moved);
    connect(ui->graphicsLabel, &QClickableLabel::Mouse_Pressed, this, &MainWindow::GraphMouse_Pressed);
    connect(ui->graphicsLabel, &QClickableLabel::Mouse_Left, this, &MainWindow::GraphMouse_Left);

    simulator = QSharedPointer<Simulator>(new Simulator(601, 601, SetFixedValues));

    arrow = MakeArrow();

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

    int valueX = x * surface->Width() / ui->graphicsLabel->width();
    int valueY = (ui->graphicsLabel->height() - y - 1) * surface->Height() / ui->graphicsLabel->height();

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

void MainWindow::GraphMouse_Pressed(int, int)
{
}

void MainWindow::GraphMouse_Left()
{
    statusBar()->hide();
}

void MainWindow::FrameUpdate()
{
    if (ui->dbGradient->checkState() == Qt::Checked)
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
    QPixmap scaledPixmap = pixmapObject.scaled(ui->graphicsLabel->width(), ui->graphicsLabel->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    if (gradient)
    {
        QPainter painter(&scaledPixmap);
        painter.setPen(Qt::black);
        //painter.setRenderHint(QPainter::Antialiasing);

        for (int y = 0; y < h; y += h/50)
        {
            for (int x = 0; x < w; x += w/50)
            {
                QVector2D v = -gradient->XYCValue(x, y); // Negated, because E-field lines go from positive to negative
                float a = atan2(v.y(), v.x());
                QMatrix rm;
                rm.rotate(a * 180 / M_PI);

                int nx = x * scaledPixmap.width() / w;
                int ny = y * scaledPixmap.height() / h;

                painter.drawPixmap(nx - arrow.width(), ny - arrow.height(), arrow.transformed(rm));
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
    simulatorThread->start();
    frameTimer->start(40);
    runTimer.start();
}

void MainWindow::on_actionSave_image_triggered()
{
    if (ui->dbGradient->checkState() == Qt::Checked)
    {
        ui->graphicsLabel->pixmap()->save("/tmp/efield_gradient.png");
    }
    else
    {
        ui->graphicsLabel->pixmap()->save("/tmp/efield_potential.png");
    }
}

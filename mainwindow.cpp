#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    static bool registered = false;
    if (!registered)
    {
        qRegisterMetaType<SimpleValueStepper>("SimpleValueStepper");

        registered = true;
    }

    ui->setupUi(this);

    connect(ui->graphicsLabel, &MouseEventLabel::Mouse_Moved, this, &MainWindow::GraphLabel_MouseMoved);
    connect(ui->graphicsLabel, &MouseEventLabel::Resized, this, &MainWindow::GraphLabel_Resized);

    connect(this, &MainWindow::StartSimulation, &mainVm, &MainVm::StartSimulation);
    connect(this, &MainWindow::StopSimulation, &mainVm, &MainVm::StopSimulation);
    connect(this, &MainWindow::UpdateVisualization, &mainVm, &MainVm::UpdateVisualization);
    connect(this, &MainWindow::RequestVisualization, &mainVm, &MainVm::RequestVisualization);
    connect(this, &MainWindow::MouseMovedOnPixmap, &mainVm, &MainVm::MouseMovedOnPixmap);

    connect(&mainVm, &MainVm::UpdateDone, this, &MainWindow::MainVm_UpdateDone);
    connect(&mainVm, &MainVm::NewVisualization, this, &MainWindow::MainVm_NewVisualization);
    connect(&mainVm, &MainVm::NewStatusMessage, this, &MainWindow::MainVm_NewStatusMessage);

#ifdef USE_VM_THREAD
    mainVm.moveToThread(&vmThread);
    vmThread.start();
#endif

    frameTimer = new QTimer(this);
    connect(frameTimer, &QTimer::timeout, this, &MainWindow::FrameUpdate);

    statusBar()->show();
}

MainWindow::~MainWindow()
{
#ifdef USE_VM_THREAD
    vmThread.quit();
    vmThread.wait();
#endif
    delete ui;
}

void MainWindow::GraphLabel_MouseMoved(const QPoint& point)
{
    emit MouseMovedOnPixmap(point, ui->graphicsLabel->size());
}

void MainWindow::GraphLabel_Resized(const QSize&)
{
    FrameUpdate();
}

void MainWindow::FrameUpdate()
{
    emit UpdateVisualization(ui->actionGradient->isChecked());
}

void MainWindow::MainVm_UpdateDone(float minValue, float maxValue)
{
    ui->heatMapLegend->SetMin(minValue);
    ui->heatMapLegend->SetMax(maxValue);

    double tickStep = ui->heatMapLegend->TickStep();
    SimpleValueStepper stepper = SimpleValueStepper(ui->actionStepped->isChecked() ? tickStep : 0);

    emit RequestVisualization(stepper, ui->graphicsLabel->width(), ui->graphicsLabel->height());
}

void MainWindow::MainVm_NewVisualization(const QPixmap& pixmap)
{
    ui->graphicsLabel->setPixmap(pixmap);
}

void MainWindow::MainVm_NewStatusMessage(const QString& message)
{
    ui->statusBar->showMessage(message);
}

void MainWindow::on_actionStart_triggered()
{
    emit StartSimulation();
    frameTimer->start(40);
}

void MainWindow::on_actionS_top_triggered()
{
    frameTimer->stop();
    emit StopSimulation();
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
    ui->heatMapLegend->SetStepped(ui->actionStepped->isChecked());

    FrameUpdate();
}

void MainWindow::on_actionRedraw_triggered()
{
    FrameUpdate();
}

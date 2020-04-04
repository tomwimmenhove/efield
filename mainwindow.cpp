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

    mainVm = new MainVm(this);

    connect(ui->graphicsLabel, &MouseEventLabel::Mouse_Moved, this, &MainWindow::GraphLabel_MouseMoved);
    connect(ui->graphicsLabel, &MouseEventLabel::Mouse_Pressed, this, &MainWindow::GraphLabel_MousePressed);
    connect(ui->graphicsLabel, &MouseEventLabel::Mouse_Released, this, &MainWindow::GraphLabel_MouseReleased);
    connect(ui->graphicsLabel, &MouseEventLabel::Mouse_DoubleClicked, this, &MainWindow::GraphLabel_MouseDoubleClicked);
    connect(ui->graphicsLabel, &MouseEventLabel::Resized, this, &MainWindow::GraphLabel_Resized);

    connect(this, &MainWindow::StartSimulation, mainVm, &MainVm::StartSimulation);
    connect(this, &MainWindow::StopSimulation, mainVm, &MainVm::StopSimulation);
    connect(this, &MainWindow::UpdateVisualization, mainVm, &MainVm::UpdateVisualization);
    connect(this, &MainWindow::RequestVisualization, mainVm, &MainVm::RequestVisualization);
    connect(this, &MainWindow::MouseMovedOnPixmap, mainVm, &MainVm::MouseMovedOnPixmap);
    connect(this, &MainWindow::MousePressedOnPixmap, mainVm, &MainVm::MousePressedOnPixmap);
    connect(this, &MainWindow::MouseReleasedFromPixmap, mainVm, &MainVm::MouseReleasedFromPixmap);
    connect(this, &MainWindow::MouseDoubleClickedOnPixmap, mainVm, &MainVm::MouseDoubleClickedOnPixmap);
    connect(this, &MainWindow::DeleteSelectedElement, mainVm, &MainVm::DeleteSelectedElement);
    connect(this, &MainWindow::EditSelectedElement, mainVm, &MainVm::EditSelectedElement);
    connect(this, &MainWindow::NewNodeElement, mainVm, &MainVm::NewNodeElement);
    connect(this, &MainWindow::NewLineElement, mainVm, &MainVm::NewLineElement);
    connect(this, &MainWindow::CancelOperation, mainVm, &MainVm::CancelOperation);
    connect(this, &MainWindow::NewSimulation, mainVm, &MainVm::NewSimulation);
    connect(this, &MainWindow::ProjectOpen, mainVm, &MainVm::ProjectOpen);
    connect(this, &MainWindow::ProjectSaveAs, mainVm, &MainVm::ProjectSaveAs);

    connect(mainVm, &MainVm::VisualizationAvailable, this, &MainWindow::MainVm_VisualizationAvailable);
    connect(mainVm, &MainVm::NewVisualization, this, &MainWindow::MainVm_NewVisualization);
    connect(mainVm, &MainVm::NewStatusMessage, this, &MainWindow::MainVm_NewStatusMessage);
    connect(mainVm, &MainVm::MouseOperationStateChanged, this, &MainWindow::MainVm_MouseOperationStateChanged);

#ifdef USE_VM_THREAD
    mainVm->moveToThread(&vmThread);
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

    graphLabelMousePos = point;
}

void MainWindow::GraphLabel_MousePressed(const QPoint& point, Qt::MouseButtons buttons)
{
    emit MousePressedOnPixmap(point, buttons, ui->graphicsLabel->size());
}

void MainWindow::GraphLabel_MouseReleased(const QPoint& point, Qt::MouseButtons buttons)
{
    emit MouseReleasedFromPixmap(point, buttons, ui->graphicsLabel->size());
}

void MainWindow::GraphLabel_MouseDoubleClicked(const QPoint& point, Qt::MouseButtons buttons)
{
    emit MouseDoubleClickedOnPixmap(point, buttons, ui->graphicsLabel->size());
}

void MainWindow::GraphLabel_Resized(const QSize&)
{
    FrameUpdate();
}

void MainWindow::FrameUpdate()
{
    emit UpdateVisualization(ui->actionGradient->isChecked());
}

void MainWindow::MainVm_VisualizationAvailable(float minValue, float maxValue)
{
    ui->heatMapLegend->SetMin(minValue);
    ui->heatMapLegend->SetMax(maxValue);

    emit RequestVisualization(ui->heatMapLegend->Stepper(), ui->graphicsLabel->size());
}

void MainWindow::MainVm_NewVisualization(const QPixmap& pixmap)
{
    ui->graphicsLabel->setPixmap(pixmap);
}

void MainWindow::MainVm_NewStatusMessage(const QString& message)
{
    ui->statusBar->showMessage(message);
}

void MainWindow::MainVm_MouseOperationStateChanged(MouseOperationStatus state)
{
    switch (state)
    {
        case MouseOperationStatus::Normal:
            ui->graphicsLabel->setCursor(Qt::ArrowCursor);
            break;
        case MouseOperationStatus::DragNode:
        case MouseOperationStatus::NewNode:
        case MouseOperationStatus::NewLineP1:
        case MouseOperationStatus::NewLineP2:
            ui->graphicsLabel->setCursor(Qt::CrossCursor);
            break;
    }
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

void MainWindow::keyPressEvent(QKeyEvent* e)
{
    if (e->modifiers() == Qt::NoModifier)
    {
        switch (e->key())
        {
            case Qt::Key_Escape:
                emit CancelOperation();
                break;
        }
    }

    QMainWindow::keyPressEvent(e);
}

void MainWindow::on_actionPlace_Node_triggered()
{
    emit NewNodeElement(graphLabelMousePos, ui->graphicsLabel->size());
}

void MainWindow::on_actionPlace_L_triggered()
{
    emit NewLineElement(graphLabelMousePos, ui->graphicsLabel->size());
}

void MainWindow::on_actionDelete_element_triggered()
{
    emit DeleteSelectedElement();
}

void MainWindow::on_action_Edit_selected_element_triggered()
{
    emit EditSelectedElement();
}

void MainWindow::on_action_New_triggered()
{
    emit NewSimulation();
    FrameUpdate();
}

void MainWindow::on_action_Open_triggered()
{
    emit ProjectOpen();
    FrameUpdate();
}

void MainWindow::on_action_Save_as_triggered()
{
    emit ProjectSaveAs();
}

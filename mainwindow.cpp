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

    connect(ui->graphicsLabel, &MouseEventLabel::mouse_Moved, this, &MainWindow::graphLabel_MouseMoved);
    connect(ui->graphicsLabel, &MouseEventLabel::mouse_Pressed, this, &MainWindow::graphLabel_MousePressed);
    connect(ui->graphicsLabel, &MouseEventLabel::mouse_Released, this, &MainWindow::graphLabel_MouseReleased);
    connect(ui->graphicsLabel, &MouseEventLabel::mouse_DoubleClicked, this, &MainWindow::graphLabel_MouseDoubleClicked);
    connect(ui->graphicsLabel, &MouseEventLabel::resized, this, &MainWindow::graphLabel_Resized);

    connect(this, &MainWindow::startSimulation, mainVm, &MainVm::startSimulation);
    connect(this, &MainWindow::stopSimulation, mainVm, &MainVm::stopSimulation);
    connect(this, &MainWindow::updateVisualization, mainVm, &MainVm::updateVisualization);
    connect(this, &MainWindow::requestVisualization, mainVm, &MainVm::requestVisualization);
    connect(this, &MainWindow::mouseMovedOnPixmap, mainVm, &MainVm::mouseMovedOnPixmap);
    connect(this, &MainWindow::mousePressedOnPixmap, mainVm, &MainVm::mousePressedOnPixmap);
    connect(this, &MainWindow::mouseReleasedFromPixmap, mainVm, &MainVm::mouseReleasedFromPixmap);
    connect(this, &MainWindow::mouseDoubleClickedOnPixmap, mainVm, &MainVm::mouseDoubleClickedOnPixmap);
    connect(this, &MainWindow::deleteSelectedElement, mainVm, &MainVm::deleteSelectedElement);
    connect(this, &MainWindow::editSelectedElement, mainVm, &MainVm::editSelectedElement);
    connect(this, &MainWindow::newNodeElement, mainVm, &MainVm::newNodeElement);
    connect(this, &MainWindow::newLineElement, mainVm, &MainVm::newLineElement);
    connect(this, &MainWindow::cancelOperation, mainVm, &MainVm::cancelOperation);
    connect(this, &MainWindow::newSimulation, mainVm, &MainVm::eewSimulation);
    connect(this, &MainWindow::projectOpen, mainVm, &MainVm::projectOpen);
    connect(this, &MainWindow::projectSaveAs, mainVm, &MainVm::projectSaveAs);

    connect(mainVm, &MainVm::visualizationAvailable, this, &MainWindow::mainVm_VisualizationAvailable);
    connect(mainVm, &MainVm::newVisualization, this, &MainWindow::mainVm_NewVisualization);
    connect(mainVm, &MainVm::newStatusMessage, this, &MainWindow::mainVm_NewStatusMessage);
    connect(mainVm, &MainVm::mouseCursorChanged, this, &MainWindow::mainVm_MouseCursorChanged);

#ifdef USE_VM_THREAD
    mainVm->moveToThread(&vmThread);
    vmThread.start();
#endif

    frameTimer = new QTimer(this);
    connect(frameTimer, &QTimer::timeout, this, &MainWindow::frameUpdate);

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

void MainWindow::graphLabel_MouseMoved(const QPoint& point)
{
    emit mouseMovedOnPixmap(point, ui->graphicsLabel->size());

    graphLabelMousePos = point;
}

void MainWindow::graphLabel_MousePressed(const QPoint& point, Qt::MouseButtons buttons)
{
    emit mousePressedOnPixmap(point, buttons, ui->graphicsLabel->size());
}

void MainWindow::graphLabel_MouseReleased(const QPoint& point, Qt::MouseButtons buttons)
{
    emit mouseReleasedFromPixmap(point, buttons, ui->graphicsLabel->size());
}

void MainWindow::graphLabel_MouseDoubleClicked(const QPoint& point, Qt::MouseButtons buttons)
{
    emit mouseDoubleClickedOnPixmap(point, buttons, ui->graphicsLabel->size());
}

void MainWindow::graphLabel_Resized(const QSize&)
{
    frameUpdate();
}

void MainWindow::frameUpdate()
{
    emit updateVisualization(ui->actionGradient->isChecked());
}

void MainWindow::mainVm_VisualizationAvailable(float minValue, float maxValue)
{
    ui->heatMapLegend->setMin(minValue);
    ui->heatMapLegend->setMax(maxValue);

    emit requestVisualization(ui->heatMapLegend->stepper(), ui->graphicsLabel->size());
}

void MainWindow::mainVm_NewVisualization(const QPixmap& pixmap)
{
    ui->graphicsLabel->setPixmap(pixmap);
}

void MainWindow::mainVm_NewStatusMessage(const QString& message)
{
    ui->statusBar->showMessage(message);
}

void MainWindow::mainVm_MouseCursorChanged(Qt::CursorShape cursor)
{
    ui->graphicsLabel->setCursor(cursor);
}

void MainWindow::on_actionStart_triggered()
{
    emit startSimulation();
    frameTimer->start(40);
}

void MainWindow::on_actionS_top_triggered()
{
    frameTimer->stop();
    emit stopSimulation();
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
    frameUpdate();
}

void MainWindow::on_actionStepped_triggered()
{
    ui->heatMapLegend->setStepped(ui->actionStepped->isChecked());

    frameUpdate();
}

void MainWindow::on_actionRedraw_triggered()
{
    frameUpdate();
}

void MainWindow::keyPressEvent(QKeyEvent* e)
{
    if (e->modifiers() == Qt::NoModifier)
    {
        switch (e->key())
        {
            case Qt::Key_Escape:
                emit cancelOperation();
                break;
        }
    }

    QMainWindow::keyPressEvent(e);
}

void MainWindow::on_actionPlace_Node_triggered()
{
    emit newNodeElement(graphLabelMousePos, ui->graphicsLabel->size());
}

void MainWindow::on_actionPlace_L_triggered()
{
    emit newLineElement(graphLabelMousePos, ui->graphicsLabel->size());
}

void MainWindow::on_actionDelete_element_triggered()
{
    emit deleteSelectedElement();
}

void MainWindow::on_action_Edit_selected_element_triggered()
{
    emit editSelectedElement();
}

void MainWindow::on_action_New_triggered()
{
    emit newSimulation();
    frameUpdate();
}

void MainWindow::on_action_Open_triggered()
{
    emit projectOpen();
    frameUpdate();
}

void MainWindow::on_action_Save_as_triggered()
{
    emit projectSaveAs();
}

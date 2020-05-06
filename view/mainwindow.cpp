#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QMessageBox>
#include <cmath>

#include "mainwindow.h"
#include "pointinputdialog.h"
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
    setWindowTitle(QCoreApplication::applicationName());

    mainVm = new MainVm();

    connect(ui->graphicsLabel, &MouseEventLabel::mouse_Moved, this, &MainWindow::graphLabel_mouseMoved);
    connect(ui->graphicsLabel, &MouseEventLabel::mouse_Pressed, this, &MainWindow::graphLabel_mousePressed);
    connect(ui->graphicsLabel, &MouseEventLabel::mouse_Released, this, &MainWindow::graphLabel_mouseReleased);
    connect(ui->graphicsLabel, &MouseEventLabel::mouse_DoubleClicked, this, &MainWindow::graphLabel_mouseDoubleClicked);
    connect(ui->graphicsLabel, &MouseEventLabel::resized, this, &MainWindow::graphLabel_resized);

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
    connect(this, &MainWindow::newCircleElement, mainVm, &MainVm::newCircleElement);
    connect(this, &MainWindow::cancelOperation, mainVm, &MainVm::cancelOperation);
    connect(this, &MainWindow::projectSave, mainVm, &MainVm::projectSave);
    connect(this, &MainWindow::undo, mainVm, &MainVm::undo);
    connect(this, &MainWindow::redo, mainVm, &MainVm::redo);
    connect(this, &MainWindow::selectAll, mainVm, &MainVm::selectAll);
    connect(this, &MainWindow::cut, mainVm, &MainVm::cut);
    connect(this, &MainWindow::copy, mainVm, &MainVm::copy);
    connect(this, &MainWindow::paste, mainVm, &MainVm::paste);
    connect(this, &MainWindow::rotate, mainVm, &MainVm::rotate);
    connect(this, &MainWindow::moveSelection, mainVm, &MainVm::moveSelection);
    connect(this, &MainWindow::setLineVoltage, mainVm, &MainVm::setLineVoltage);
    connect(this, &MainWindow::setCircleVoltage, mainVm, &MainVm::setCircleVoltage);
    connect(this, &MainWindow::setNodePosition, mainVm, &MainVm::setNodePosition);
    connect(this, &MainWindow::moveSelectionRequested, mainVm, &MainVm::moveSelectionRequested);

    connect(mainVm, &MainVm::criticalMessage, this, &MainWindow::mainVm_criticalMessage);
    connect(mainVm, &MainVm::visualizationAvailable, this, &MainWindow::mainVm_visualizationAvailable);
    connect(mainVm, &MainVm::newVisualization, this, &MainWindow::mainVm_newVisualization);
    connect(mainVm, &MainVm::newStatusMessage, this, &MainWindow::mainVm_newStatusMessage);
    connect(mainVm, &MainVm::updateMouseCursor, this, &MainWindow::mainVm_updateMouseCursor);
    connect(mainVm, &MainVm::undoStackUpdated, this, &MainWindow::mainVm_undoStackUpdated);
    connect(mainVm, &MainVm::projectStatusUpdate, this, &MainWindow::mainVm_projectStatusUpdate);
    connect(mainVm, &MainVm::editLine, this, &MainWindow::mainVm_editLine);
    connect(mainVm, &MainVm::editCircle, this, &MainWindow::mainVm_editCircle);
    connect(mainVm, &MainVm::editNode, this, &MainWindow::mainVm_editNode);
    connect(mainVm, &MainVm::moveSelectionDialog, this, &MainWindow::mainVm_moveSelectionDialog);

    /* Save as */
    connect(mainVm, &MainVm::saveDialog, this, &MainWindow::mainVm_saveDialog);

    connect(this, &MainWindow::saveAs, mainVm, &MainVm::saveAs);

    /* Closing */
    connect(this, &MainWindow::closeRequested, mainVm, &MainVm::closeRequested);
    connect(this, &MainWindow::saveBeforeClose, mainVm, &MainVm::saveBeforeClose);
    connect(this, &MainWindow::closeApplication, mainVm, &MainVm::closeApplication);
    connect(this, &MainWindow::saveAsBeforeClose, mainVm, &MainVm::saveAsBeforeClose);

    connect(mainVm, &MainVm::askSaveBeforeClose, this, &MainWindow::mainVm_askSaveBeforeClose);
    connect(mainVm, &MainVm::saveDialogBeforeClose, this, &MainWindow::mainVm_saveDialogBeforeClose);

    /* Opening */
    connect(this, &MainWindow::projectOpenRequested, mainVm, &MainVm::projectOpenRequested);
    connect(this, &MainWindow::saveBeforeOpen, mainVm, &MainVm::saveBeforeOpen);
    connect(this, &MainWindow::projectOpen, mainVm, &MainVm::projectOpen);
    connect(this, &MainWindow::saveAsBeforeOpen, mainVm, &MainVm::saveAsBeforeOpen);

    connect(mainVm, &MainVm::openDialog, this, &MainWindow::mainVm_openDialog);
    connect(mainVm, &MainVm::saveDialogBeforeOpen, this, &MainWindow::mainVm_saveDialogBeforeOpen);
    connect(mainVm, &MainVm::askSaveBeforeOpen, this, &MainWindow::mainVm_askSaveBeforeOpen);

    /* New */
    connect(this, &MainWindow::newProjectRequested, mainVm, &MainVm::newProjectRequested);
    connect(this, &MainWindow::saveBeforeNewProject, mainVm, &MainVm::saveBeforeNewProject);
    connect(this, &MainWindow::newProject, mainVm, &MainVm::newProject);
    connect(this, &MainWindow::saveAsBeforeNewProject, mainVm, &MainVm::saveAsBeforeNewProject);

    connect(mainVm, &MainVm::newProjectDialog, this, &MainWindow::mainVm_newProjectDialog);
    connect(mainVm, &MainVm::askSaveBeforeNewProject, this, &MainWindow::mainVm_askSaveBeforeNewProject);
    connect(mainVm, &MainVm::saveDialogBeforeNewProject, this, &MainWindow::mainVm_saveDialogBeforeNewProject);

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

void MainWindow::graphLabel_mouseMoved(const QPoint& point, Qt::MouseButtons buttons)
{
    emit mouseMovedOnPixmap(point, buttons, ui->graphicsLabel->size());

    graphLabelMousePos = point;
}

void MainWindow::graphLabel_mousePressed(const QPoint& point, Qt::MouseButtons buttons)
{
    emit mousePressedOnPixmap(point, buttons, ui->graphicsLabel->size());
}

void MainWindow::graphLabel_mouseReleased(const QPoint& point, Qt::MouseButtons buttons)
{
    emit mouseReleasedFromPixmap(point, buttons, ui->graphicsLabel->size());
}

void MainWindow::graphLabel_mouseDoubleClicked(const QPoint& point, Qt::MouseButtons buttons)
{
    emit mouseDoubleClickedOnPixmap(point, buttons, ui->graphicsLabel->size());
}

void MainWindow::graphLabel_resized(const QSize&)
{
    frameUpdate();
}

void MainWindow::mainVm_criticalMessage(const QString& topic, const QString& message)
{
    QMessageBox::critical(this, topic, message);
}

void MainWindow::frameUpdate()
{
    emit updateVisualization(ui->actionGradient->isChecked());
}

QString MainWindow::saveAsDialog()
{
    return QFileDialog::getSaveFileName(this, tr("Save Project"), "",
                                        tr("E-Field Sim files (*.efs)"));
}

void MainWindow::mainVm_visualizationAvailable(float minValue, float maxValue)
{
    ui->heatMapLegend->setMin(minValue);
    ui->heatMapLegend->setMax(maxValue);

    emit requestVisualization(ui->heatMapLegend->stepper(), ui->graphicsLabel->size());
}

void MainWindow::mainVm_newVisualization(const QPixmap& pixmap)
{
    ui->graphicsLabel->setPixmap(pixmap);
}

void MainWindow::mainVm_newStatusMessage(const QString& message)
{
    ui->statusBar->showMessage(message);
}

void MainWindow::mainVm_updateMouseCursor(Qt::CursorShape cursor)
{
    ui->graphicsLabel->setCursor(cursor);
}

void MainWindow::mainVm_undoStackUpdated(bool canUndo, const QString& undoName, bool canRedo, const QString& redoName)
{
    ui->action_Undo->setEnabled(canUndo);
    ui->action_Undo->setText(tr("Undo %1").arg(undoName));

    ui->action_Redo->setEnabled(canRedo);
    ui->action_Redo->setText(tr("Redo %1").arg(redoName));
}

void MainWindow::mainVm_projectStatusUpdate(const QString& filename, bool altered)
{
    ui->action_Save->setEnabled(altered);
    ui->action_Save_as->setEnabled(altered);

    if (filename.isEmpty())
    {
        setWindowTitle(QString("%1%2")
                          .arg(QCoreApplication::applicationName())
                          .arg(altered ? " *" : ""));
        return;
    }

    setWindowTitle(QString("[%1]- %2%3")
                      .arg(QFileInfo(filename).fileName())
                      .arg(QCoreApplication::applicationName())
                   .arg(altered ? " *" : ""));
}

void MainWindow::mainVm_editNode(int id, const QPoint& defaultPosition,
                                 const QPoint& minPosition,
                                 const QPoint& maxPosition)
{
    PointInputDialog d(QWidget::tr("Node coordinates"),
                       defaultPosition, minPosition, maxPosition, this);
    if (d.exec() != QDialog::Accepted)
        return;

    emit setNodePosition(id, defaultPosition, d.point());
}

void MainWindow::mainVm_editLine(int id, float defaultVoltage)
{
    bool ok;

    double voltage = QInputDialog::getDouble(this, QWidget::tr("Edit line"),
                                    QWidget::tr("Voltage: "),  defaultVoltage, -1e100, 1e100, 1, &ok);
    if (!ok)
        return;

    emit setLineVoltage(id, defaultVoltage, voltage);
}

void MainWindow::mainVm_editCircle(int id, float defaultVoltage)
{
    bool ok;

    double voltage = QInputDialog::getDouble(this, QWidget::tr("Edit circle"),
                                    QWidget::tr("Voltage: "),  defaultVoltage, -1e100, 1e100, 1, &ok);
    if (!ok)
        return;

    emit setCircleVoltage(id, defaultVoltage, voltage);
}

void MainWindow::mainVm_saveDialog()
{
    QString fileName = saveAsDialog();
    if (!fileName.isEmpty())
        emit saveAs(fileName);
}

int MainWindow::askSaveDialog()
{
    QMessageBox msgBox;
    msgBox.setText("The current project has been modified.");
    msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);

    return msgBox.exec();
}

void MainWindow::mainVm_askSaveBeforeClose()
{
    switch(askSaveDialog())
    {
        case QMessageBox::Discard:
            emit closeApplication();
            break;
        case QMessageBox::Save:
            emit saveBeforeClose();
            break;
        case QMessageBox::Cancel:
        default:
            return;
    }
}

void MainWindow::mainVm_saveDialogBeforeClose()
{
    QString fileName = saveAsDialog();
    if (!fileName.isEmpty())
        emit saveAsBeforeClose(fileName);
}

void MainWindow::mainVm_askSaveBeforeOpen()
{
    switch(askSaveDialog())
    {
        case QMessageBox::Discard:
            mainVm_openDialog();
            break;
        case QMessageBox::Save:
            emit saveBeforeOpen();
            break;
        case QMessageBox::Cancel:
        default:
            return;
    }
}

void MainWindow::mainVm_saveDialogBeforeOpen()
{
    QString fileName = saveAsDialog();
    if (!fileName.isEmpty())
        emit saveAsBeforeOpen(fileName);
}

void MainWindow::mainVm_openDialog()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Project"), "",
                                                    tr("E-Field Sim files (*.efs)"));

    if (!filename.isEmpty())
    {
        emit projectOpen(filename);
        emit updateVisualization(ui->actionGradient->isChecked());
    }
}

void MainWindow::mainVm_askSaveBeforeNewProject()
{
    switch(askSaveDialog())
    {
        case QMessageBox::Discard:
            mainVm_newProjectDialog();
            break;
        case QMessageBox::Save:
            emit saveBeforeNewProject();
            break;
        case QMessageBox::Cancel:
        default:
            return;
    }
}

void MainWindow::mainVm_saveDialogBeforeNewProject()
{
    QString fileName = saveAsDialog();
    if (!fileName.isEmpty())
        emit saveAsBeforeNewProject(fileName);
}

void MainWindow::mainVm_newProjectDialog()
{
    PointInputDialog d("Scene size", QPoint(256, 256), QPoint(1, 1), QPoint(1024, 1024), this);
    if (d.exec() != QDialog::Accepted)
        return;

    emit newProject(d.size());
    emit updateVisualization(ui->actionGradient->isChecked());
}

void MainWindow::on_action_Open_triggered()
{
    emit projectOpenRequested();
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

            case Qt::Key_Space:
                emit rotate(-45.0f);
                break;
        }
    }

    QMainWindow::keyPressEvent(e);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (shouldClose)
        event->accept();
    else
    {
        event->ignore();
        emit closeRequested();
    }
}

void MainWindow::on_actionPlace_Node_triggered()
{
    emit newNodeElement(graphLabelMousePos, ui->graphicsLabel->size());
}

void MainWindow::on_actionPlace_Circle_triggered()
{
    emit newCircleElement(graphLabelMousePos, ui->graphicsLabel->size());
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
    emit newProjectRequested();
}

void MainWindow::on_action_Save_triggered()
{
    emit projectSave();
}

void MainWindow::on_action_Save_as_triggered()
{
    mainVm_saveDialog();
}

void MainWindow::on_action_Undo_triggered()
{
    emit undo();
}

void MainWindow::on_action_Redo_triggered()
{
    emit redo();
}

void MainWindow::on_actionSelect_all_triggered()
{
    emit selectAll();
}

void MainWindow::on_actionCut_triggered()
{
    emit cut();
}

void MainWindow::on_action_Copy_triggered()
{
    emit copy();
}

void MainWindow::on_action_Paste_triggered()
{
    emit paste();
}

void MainWindow::on_action_Rotate_selection_triggered()
{
    bool ok;
    double rot = QInputDialog::getDouble(this, QWidget::tr("Rotate selection"),
                                  QWidget::tr("Rotation in degrees: "),  0, -360, 360, 1, &ok);
    if (!ok)
        return;

    emit rotate(rot);
}

void MainWindow::mainVm_moveSelectionDialog(const QPoint& maxPoint)
{
    PointInputDialog d(QWidget::tr("Move selection"),
                       QPoint(0, 0), QPoint(0, 0), maxPoint, this);
    if (d.exec() != QDialog::Accepted)
        return;

    emit moveSelection(d.point());
}

void MainWindow::on_action_Move_selection_triggered()
{
    emit moveSelectionRequested();
}

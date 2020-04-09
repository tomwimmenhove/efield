#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QElapsedTimer>

#include "viewmodel/mainvm.h"

//#define USE_VM_THREAD

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void graphLabel_MouseMoved(const QPoint& point);
    void graphLabel_MousePressed(const QPoint& point, Qt::MouseButtons buttons);
    void graphLabel_MouseReleased(const QPoint& point, Qt::MouseButtons buttons);
    void graphLabel_MouseDoubleClicked(const QPoint& point, Qt::MouseButtons buttons);
    void graphLabel_Resized(const QSize& size);

    void mainVm_VisualizationAvailable(float minValue, float maxValue);
    void mainVm_NewVisualization(const QPixmap& pixmap);
    void mainVm_NewStatusMessage(const QString& message);
    void mainVm_MouseCursorChanged(Qt::CursorShape cursor);

    void on_actionStart_triggered();
    void on_actionS_top_triggered();
    void on_actionSave_Image_triggered();
    void on_actionGradient_triggered();
    void on_actionStepped_triggered();
    void on_actionRedraw_triggered();
    void on_actionPlace_Node_triggered();
    void on_actionPlace_L_triggered();
    void on_actionDelete_element_triggered();
    void on_action_Edit_selected_element_triggered();
    void on_action_New_triggered();
    void on_action_Open_triggered();
    void on_action_Save_as_triggered();

signals:
    void startSimulation();
    void stopSimulation();
    void updateVisualization(bool useGradiant);
    void requestVisualization(const SimpleValueStepper& stepper, const QSize& size);
    void mouseMovedOnPixmap(const QPoint& mousePos, const QSize& labelSize);
    void mousePressedOnPixmap(const QPoint& mousePos, Qt::MouseButtons buttons, const QSize& labelSize);
    void mouseReleasedFromPixmap(const QPoint& mousePos, Qt::MouseButtons buttons, const QSize& labelSize);
    void mouseDoubleClickedOnPixmap(const QPoint& mousePos, Qt::MouseButtons buttons, const QSize& labelSize);
    void deleteSelectedElement();
    void editSelectedElement();
    void newSimulation();
    void newNodeElement(const QPoint& mousePos, const QSize& labelSize);
    void newLineElement(const QPoint& mousePos, const QSize& labelSize);
    void cancelOperation();
    void projectOpen();
    void projectSaveAs();

protected:
    void keyPressEvent(QKeyEvent *e);

private:
    void frameUpdate();

    MainVm* mainVm;
#ifdef USE_VM_THREAD
    QThread vmThread;
#endif

    QPoint graphLabelMousePos;
    QTimer* frameTimer;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

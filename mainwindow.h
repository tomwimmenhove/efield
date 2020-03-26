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
    void GraphLabel_MouseMoved(const QPoint& point);
    void GraphLabel_Resized(const QSize& size);

    void MainVm_UpdateDone(float minValue, float maxValue);
    void MainVm_NewVisualization(const QPixmap& pixmap);
    void MainVm_NewStatusMessage(const QString& message);

    void on_actionStart_triggered();
    void on_actionS_top_triggered();
    void on_actionSave_Image_triggered();
    void on_actionGradient_triggered();
    void on_actionStepped_triggered();
    void on_actionRedraw_triggered();

signals:
    void StartSimulation();
    void StopSimulation();
    void UpdateVisualization(bool useGradiant);
    void RequestVisualization(const SimpleValueStepper& stepper, int width, int height);
    void MouseMovedOnPixmap(QPoint mousePos, QSize pixmapSize);

private:
    void FrameUpdate();

    MainVm mainVm;
#ifdef USE_VM_THREAD
    QThread vmThread;
#endif

    QTimer* frameTimer;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

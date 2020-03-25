#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSharedPointer>
#include <QElapsedTimer>
#include <QExposeEvent>

#include "model/simulator.h"
#include "model/simulatorthread.h"
#include "model/gradientsurface.h"

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
    void FrameUpdate();

    void GraphMouse_Moved(int x, int y);

    void on_actionStart_triggered();
    void on_actionS_top_triggered();
    void on_actionSave_Image_triggered();
    void on_actionGradient_triggered();
    void on_actionStepped_triggered();

    void on_actionRedraw_triggered();

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void StartSimulation();
    void StopSimulation();

    SimulatorThread* simulatorThread;
    static void SetFixedValues(FloatSurface& surface);

    QSharedPointer<Simulator> simulator;

    QSharedPointer<FloatSurface> surface;
    QSharedPointer<GradientSurface> gradient;

    Ui::MainWindow *ui;
    QTimer* frameTimer;
    QElapsedTimer runTimer;
    int frames = 0;
};

#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "model/simulator.h"

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
    void on_pushButton_clicked();
    void FrameUpdate();

    void GraphMouse_Moved(int x, int y);
    void GraphMouse_Pressed(int x, int y);
    void GraphMouse_Left();

private:
    void SetFixedValues();

    Simulator simulator = Simulator(250, 250);

    Ui::MainWindow *ui;
    QTimer* frameTimer;
};

#endif // MAINWINDOW_H

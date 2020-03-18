#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

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
    void IterateSimulation();

    void GraphMouse_Moved(int x, int y);
    void GraphMouse_Pressed(int x, int y);
    void GraphMouse_Left();

private:
    void SetFixedValues();
    float* SwitchBuf();
    inline float& XYValue(int x, int y) { return curBuf[x + y * w]; }
    inline float& XYValue(float* buf, int x, int y) { return buf[x + y * w]; }
    float SlowValueAverager(float* buf, int x, int y);

    Ui::MainWindow *ui;
    float* values[2];
    int w = 250;
    int h = 250;

    QTimer* frameTimer;

    float* curBuf;
    int curBufIdx = 0;
};

#endif // MAINWINDOW_H

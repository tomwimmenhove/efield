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
    void graphLabel_mouseMoved(const QPoint& point, Qt::MouseButtons buttons);
    void graphLabel_mousePressed(const QPoint& point, Qt::MouseButtons buttons);
    void graphLabel_mouseReleased(const QPoint& point, Qt::MouseButtons buttons);
    void graphLabel_mouseDoubleClicked(const QPoint& point, Qt::MouseButtons buttons);
    void graphLabel_resized(const QSize& size);

    void mainVm_visualizationAvailable(float minValue, float maxValue);
    void mainVm_newVisualization(const QPixmap& pixmap);
    void mainVm_newStatusMessage(const QString& message);
    void mainVm_updateMouseCursor(Qt::CursorShape cursor);
    void mainVm_undoStackUpdated(bool canUndo, const QString& undoName, bool canRedo, const QString& redoName);
    void mainVm_projectStatusUpdate(const QString& filename, bool altered);
    void mainVm_editNode(int id, const QPoint& defaultPosition, const QPoint& minPosition, const QPoint& maxPosition);
    void mainVm_editLine(int id, float defaultVoltage);

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
    void on_action_Save_triggered();
    void on_action_Save_as_triggered();
    void on_action_Undo_triggered();
    void on_action_Redo_triggered();
    void on_actionSelect_all_triggered();
    void on_action_Copy_triggered();
    void on_action_Paste_triggered();
    void on_actionCut_triggered();
    void on_action_Rotate_selection_triggered();

signals:
    void startSimulation();
    void stopSimulation();
    void updateVisualization(bool useGradiant);
    void requestVisualization(const SimpleValueStepper& stepper, const QSize& size);
    void mouseMovedOnPixmap(const QPoint& mousePos, Qt::MouseButtons buttons, const QSize& labelSize);
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
    void projectSave();
    void projectSaveAs();
    void undo();
    void redo();
    void selectAll();
    void cut();
    void copy();
    void paste();
    void rotate(double rot);
    void closeRequested();

    void setLineVoltage(int id, float oldVoltage, float newVoltage);
    void setNodePosition(int id, QPoint oldPosition, QPoint newPosition);

protected:
    void keyPressEvent(QKeyEvent *e);
    void closeEvent(QCloseEvent *event);

private:
    void frameUpdate();

    MainVm* mainVm;
#ifdef USE_VM_THREAD
    QThread vmThread;
#endif

    bool shouldClose = false;
    QPoint graphLabelMousePos;
    QTimer* frameTimer;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

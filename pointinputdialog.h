#ifndef POINTINPUTDIALOG_H
#define POINTINPUTDIALOG_H

#include <QDialog>
#include <QPoint>

namespace Ui {
class PointInputDialog;
}

class PointInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PointInputDialog(QPoint point, QPoint minPoint, QPoint maxPoint, QWidget *parent = 0);
    ~PointInputDialog();

    QPoint Point() const;

private:
    Ui::PointInputDialog *ui;
};

#endif // POINTINPUTDIALOG_H

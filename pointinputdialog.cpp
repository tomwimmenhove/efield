#include "pointinputdialog.h"
#include "ui_pointinputdialog.h"

PointInputDialog::PointInputDialog(QPoint point, QPoint minPoint, QPoint maxPoint, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PointInputDialog)
{
    ui->setupUi(this);

    ui->spinBoxX->setValue(point.x());
    ui->spinBoxY->setValue(point.y());

    ui->spinBoxX->setMinimum(minPoint.x());
    ui->spinBoxY->setMinimum(minPoint.y());

    ui->spinBoxX->setMaximum(maxPoint.x());
    ui->spinBoxY->setMaximum(maxPoint.y());
}

PointInputDialog::~PointInputDialog()
{
    delete ui;
}

QPoint PointInputDialog::Point() const
{
    return QPoint(ui->spinBoxX->value(), ui->spinBoxY->value());
}

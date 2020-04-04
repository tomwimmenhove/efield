#include "pointinputdialog.h"
#include "ui_pointinputdialog.h"

PointInputDialog::PointInputDialog(const QString& title, const QPoint& point, const QPoint& minPoint, const QPoint& maxPoint, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::PointInputDialog)
{
    ui->setupUi(this);

    setWindowTitle(title);

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

QSize PointInputDialog::Size() const
{
    return QSize(ui->spinBoxX->value(), ui->spinBoxY->value());
}

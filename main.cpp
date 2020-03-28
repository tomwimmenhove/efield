#include "mainwindow.h"
#include <QApplication>

#include <QSharedPointer>
#include <QPoint>
#include <QDebug>

#include "grphxelements/sharedint.h"
#include "grphxelements/lineelement.h"

int main(int argc, char *argv[])
{
    LineElement<float> l2(SharedNode(1, 2), SharedNode(3, 4), 42.42);

    SharedNode n1(12, 34);

    qDebug() << "n1: " << *n1;
    SharedNode n2 = n1;//(1, 2);

    //n2 = n1;

    n1 = QPoint(34, 12);

    QPoint p1 = n1;

    qDebug() << "p1: " << p1;
    qDebug() << "n2: " << *n2;

    //return 0;
    SharedInt ia = 42;
    SharedInt ib = ia;

    ia = 1234;

    qDebug() << "a: " << *ia;
    qDebug() << "b: " << *ib;

    //return 0;

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

#include "mainwindow.h"
#include <QApplication>

//#include "model/surface.h"
//#include <QDebug>

int main(int argc, char *argv[])
{
//    Surface s(2, 2, 0);

//#if 1
//    s.XYValue(0,  0) = 1;
//    s.XYValue(1,  0) = .5;
//    s.XYValue(0,  1) = .5;
//    s.XYValue(1,  1) = 0;
//#else
//    s.XYValue(0,  0) = 1;
//    s.XYValue(1,  0) = 1;
//    s.XYValue(0,  1) = 0;
//    s.XYValue(1,  1) = 0;
//#endif

//    auto v = s.ToGradient();

//    qDebug() << v->XYCValue(0, 0);

//    return 0;

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

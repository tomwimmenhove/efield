#include "mainwindow.h"
#include <QApplication>

#include "model/surface.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    FloatSurface s(2, 2, 0);

#if 1
    s.XYValue(0,  0) = 1;
    s.XYValue(1,  0) = .5;
    s.XYValue(0,  1) = .5;
    s.XYValue(1,  1) = 0;
#else
    s.XYValue(0,  0) = 1;
    s.XYValue(1,  0) = 1;
    s.XYValue(0,  1) = 0;
    s.XYValue(1,  1) = 0;
#endif

    GradientSurface v(s);

    qDebug() << v.XYCValue(0, 0) << " -- " << v.XYCValue(0, 0).length();

    //return 0;

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

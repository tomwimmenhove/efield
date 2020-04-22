#include "view/mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("E-FieldSim");
    MainWindow w;
    w.show();

    return a.exec();
}

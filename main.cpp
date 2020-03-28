#include "mainwindow.h"
#include <QApplication>

#include "graphics/sharedint.h"

#include "util/refcounted.h"





int main(int argc, char *argv[])
{
    SharedInt i1(42);

    i1->Use();

    SharedInt i2 = i1;

    i2->Use();

    i2 = 42;

//    SharedInt i1 = 42;
//    SharedInt i2 = i1;

//    i2 = 43;

    qDebug() << ((int) i1);
    qDebug() << i2->RefCount();

//    Refcounted<int> aaa(12);



    //return 0;
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

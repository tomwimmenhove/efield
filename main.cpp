#include "mainwindow.h"
#include <QApplication>

//#include "util/undostack.h"

//static int q = 42;
//UndoStack us;

//void setQ(int i)
//{
//    int oldQ = q;

//    auto undoFunc = [oldQ]() { q = oldQ; };
//    auto redoFunc = [i]() { q = i; };

//    us.add(UndoItem(QString("Undo setting q from %1 to %2").arg(oldQ).arg(i), undoFunc, redoFunc));

//    qDebug() << "Setting Q from " << q << " to " << i;

//    redoFunc();
//}

int main(int argc, char *argv[])
{
//    qDebug() << "before setQ " << q;

//    for (int i = 0; i < 10; i++)
//        setQ(i);

//    for (int i = 0; i < 5; i++)
//    {
//        us.undo();

//        qDebug() << "after undo " << q;
//    }

////    us.redo();

//    setQ(100);

//    for (int i = 0; i < 6; i++)
//    {
//        us.undo();

//        qDebug() << "after undo " << q;
//    }



//    return 0;

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

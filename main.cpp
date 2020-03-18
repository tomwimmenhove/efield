#include "mainwindow.h"
#include <QApplication>
#ifdef _OPENMP
#include <omp.h>
#endif

int main(int argc, char *argv[])
{
#ifdef _OPENMP
    int threadNum = omp_get_thread_num();
    printf("This is thread %d\n", threadNum);
#endif

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

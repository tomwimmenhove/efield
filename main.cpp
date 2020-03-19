#include "mainwindow.h"
#include <QApplication>
#ifdef _OPENMP
#include <omp.h>
#endif

int main(int argc, char *argv[])
{
#ifdef _OPENMP
    int maxThreads = omp_get_max_threads();
    omp_set_num_threads(maxThreads);
#endif

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

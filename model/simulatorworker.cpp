#include "simulatorworker.h"

#include <QDebug>
#include <QMutex>
#include <QMutexLocker>

#ifdef _OPENMP
#include <omp.h>

void SimulatorWorker::runParallel()
{
    simulator->preIterateSimulationChunk();

    QAtomicInt doCancel = false;

    int n = simulator->numChunks();
    omp_set_num_threads(nThreads);
#pragma omp parallel
    {
        int id = omp_get_thread_num();
        int numThreads = omp_get_num_threads();
        int istart = id * n / numThreads;
        int iend = (id + 1) * n / numThreads;

        if (id == numThreads - 1)
            iend = n - 1;

        qDebug() << "Simulation thread " << id << " starting";

        while (!doCancel)
        {
            simulator->iterateSimulationChunk(istart, iend);

#pragma omp barrier
            if (id == 0)
            {
                simulator->postIterateSimulationChunk();
                iters++;

                if (cancel)
                {
                    doCancel = true;
                }
                else
                {
                    simulator->preIterateSimulationChunk();
                }
            }
#pragma omp barrier
        }

        qDebug() << "Simulation thread " << id << " finished";
    }
}
#endif

void SimulatorWorker::runSequential()
{
    qDebug() << "Simulation starting";
    while (!cancel)
    {
        simulator->iterateSimulation();
        iters++;
    }
    qDebug() << "Simulation finished";
}

void SimulatorWorker::run()
{
    cancel = false;

    iters = 0;

#ifdef _OPENMP
    if (nThreads > 1)
        runParallel();
    else
#endif
        runSequential();
}

#include "simulatorworker.h"

#include <QDebug>
#include <QMutex>
#include <QMutexLocker>

#ifdef _OPENMP
#include <omp.h>

void SimulatorWorker::RunParallel()
{
    simulator->PreIterateSimulationChunk();

    QAtomicInt doCancel = false;

    int n = simulator->NumChunks();
    omp_set_num_threads(numThreads);
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
            simulator->IterateSimulationChunk(istart, iend);

#pragma omp barrier
            if (id == 0)
            {
                simulator->PostIterateSimulationChunk();
                iterations++;

                if (cancel)
                {
                    doCancel = true;
                }
                else
                {
                    simulator->PreIterateSimulationChunk();
                }
            }
#pragma omp barrier
        }

        qDebug() << "Simulation thread " << id << " finished";
    }
}
#endif

void SimulatorWorker::RunSequential()
{
    qDebug() << "Simulation starting";
    while (!cancel)
    {
        simulator->IterateSimulation();
        iterations++;
    }
    qDebug() << "Simulation finished";
}

void SimulatorWorker::Run()
{
    cancel = false;

    iterations = 0;

#ifdef _OPENMP
    if (numThreads > 1)
        RunParallel();
    else
#endif
        RunSequential();
}

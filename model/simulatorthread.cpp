#include <QDebug>
#include <QMutex>
#include <QMutexLocker>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "simulatorthread.h"

SimulatorThread::SimulatorThread(QSharedPointer<Simulator> simulator)
    : simulator(simulator)
{
    static bool registered = false;

    if (!registered)
    {
        qRegisterMetaType<QSharedPointer<Surface>>("sharedSurface");

        registered = true;
    }
}

#ifdef _OPENMP
void SimulatorThread::RunParallel()
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

        while (!doCancel)
        {
            simulator->IterateSimulationChunk(istart, iend);

#pragma omp barrier
            if (id == 0)
            {
                simulator->PostIterateSimulationChunk();

                if (requestSurface)
                {
                    requestSurface--;
                    emit NewSurface(QSharedPointer<Surface>(new Surface(simulator->CurrentSurface())));
                }

                iterations++;

                if (cancel)
                {
                    doCancel = true;
                    break;
                }

                simulator->PreIterateSimulationChunk();
            }
#pragma omp barrier
        }
    }
}
#endif

void SimulatorThread::RunSequential()
{
    while (!cancel)
    {
        simulator->IterateSimulation();

        if (requestSurface)
        {
            requestSurface--;
            emit NewSurface(QSharedPointer<Surface>(new Surface(simulator->CurrentSurface())));
        }

        iterations++;
    }
}

void SimulatorThread::run()
{
    cancel = false;

#ifdef _OPENMP
    if (numThreads > 1)
        RunParallel();
    else
#endif
        RunSequential();
}

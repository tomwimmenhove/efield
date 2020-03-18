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

void SimulatorThread::run()
{
    cancel = false;

    while (!cancel)
    {
        simulator->IterateSimulation();

        if (requestSurface)
        {
            requestSurface--;
            emit NewSurface(simulator->CurrentSurface().Clone());
        }
    }
}

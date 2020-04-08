#include <limits>
#include <QDebug>
#include <QElapsedTimer>

#include "simulator.h"

Simulator::Simulator(const QSize& size, std::function<void(FloatSurface&)> updateBoundariesHandler)
    : w(size.width()), h(size.height()), updateBoundariesHandler(updateBoundariesHandler)
{
    surfaceMutexex[curBufIdx].lock();

    updateBoundariesHandler(currentSurface());
}

QSharedPointer<FloatSurface> Simulator::cloneSurface()
{
    QMutexLocker locker(&surfaceMutexex[curBufIdx ^ 1]);

    return QSharedPointer<FloatSurface>(new FloatSurface(otherSurface()));
}

void Simulator::switchSurface()
{
    surfaceMutexex[curBufIdx].unlock();
    curBufIdx ^= 1;
    surfaceMutexex[curBufIdx].lock();
}

void Simulator::iterateSimulation()
{
    preIterateSimulationChunk();
    iterateSimulationChunk(1, h - 1); // First and last are done by PreIterateSimulationChunk()
    postIterateSimulationChunk();
}

void Simulator::preIterateSimulationChunk()
{
    FloatSurface& oldSurface = currentSurface();
    switchSurface();
    FloatSurface& curSurface = currentSurface();

    for (int x = 0; x < w; x++)
        curSurface.setValue(x, 0, slowValueAverager(oldSurface, x, 0));

    for (int x = 0; x < w; x++)
        curSurface.setValue(x, h - 1, slowValueAverager(oldSurface, x, h - 1));

    for (int y = 0; y < h; y++)
        curSurface.setValue(0, y, slowValueAverager(oldSurface, 0, y));

    for (int y = 0; y < h; y++)
        curSurface.setValue(w - 1, y, slowValueAverager(oldSurface, w - 1, y));
}

void Simulator::iterateSimulationChunk(int startChunk, int endChunk)
{
    FloatSurface& oldSurface = otherSurface();

    for (int y = startChunk >= 1 ? startChunk : 1; y < endChunk; y++)
    {
        for (int x = 1; x < w - 1; x++)
        {
            currentSurface().setValue(x, y, (oldSurface.value(x - 1, y - 1) +
                                               oldSurface.value(x    , y - 1) +
                                               oldSurface.value(x + 1, y - 1) +

                                               oldSurface.value(x - 1, y    ) +
                                               oldSurface.value(x + 1, y    ) +

                                               oldSurface.value(x - 1, y + 1) +
                                               oldSurface.value(x    , y + 1) +
                                               oldSurface.value(x + 1, y + 1)
                                               ) / 8.0f);
        }
    }
}

float Simulator::slowValueAverager(FloatSurface& surface, int x, int y)
{
    float total = 0;
    int n = 0;
    if (y > 0)
    {
        total += surface.value(x, y - 1);
        n++;
        if (x < w - 2)
        {
            total += surface.value(x + 1, y - 1);
            n++;
        }
    }

    if (x > 0)
    {
        total += surface.value(x - 1, y);
        n++;
        if (y < h - 2)
        {
            total += surface.value(x - 1, y + 1);
            n++;
        }
        if (y > 0)
        {
            total += surface.value(x - 1, y - 1);
            n++;
        }
    }

    if (x < w - 2)
    {
        total += surface.value(x + 1, y    );
        n++;
        if (y < h - 2)
        {
            total += surface.value(x + 1, y + 1);
            n++;
        }
    }

    if (y < h - 2)
    {
        total += surface.value(x, y + 1);
        n++;
    }


    return total / (float) n;
}


#include <limits>
#include <QDebug>
#include <QElapsedTimer>

#include "simulator.h"

Simulator::Simulator(const QSize& size, std::function<void(FloatSurface&)> updateBoundariesHandler)
    : w(size.width()), h(size.height()), updateBoundariesHandler(updateBoundariesHandler)
{
    surfaceMutexex[curBufIdx].lock();

    updateBoundariesHandler(CurrentSurface());
}

QSharedPointer<FloatSurface> Simulator::CloneSurface()
{
    QMutexLocker locker(&surfaceMutexex[curBufIdx ^ 1]);

    return QSharedPointer<FloatSurface>(new FloatSurface(OtherSurface()));
}

void Simulator::SwitchSurface()
{
    surfaceMutexex[curBufIdx].unlock();
    curBufIdx ^= 1;
    surfaceMutexex[curBufIdx].lock();
}

void Simulator::IterateSimulation()
{
    PreIterateSimulationChunk();
    IterateSimulationChunk(1, h - 1); // First and last are done by PreIterateSimulationChunk()
    PostIterateSimulationChunk();
}

void Simulator::PreIterateSimulationChunk()
{
    FloatSurface& oldSurface = CurrentSurface();
    SwitchSurface();
    FloatSurface& curSurface = CurrentSurface();

    for (int x = 0; x < w; x++)
        curSurface.XYValue(x, 0) = SlowValueAverager(oldSurface, x, 0);

    for (int x = 0; x < w; x++)
        curSurface.XYValue(x, h - 1) = SlowValueAverager(oldSurface, x, h - 1);

    for (int y = 0; y < h; y++)
        curSurface.XYValue(0, y) = SlowValueAverager(oldSurface, 0, y);

    for (int y = 0; y < h; y++)
        curSurface.XYValue(w - 1, y) = SlowValueAverager(oldSurface, w - 1, y);
}

void Simulator::IterateSimulationChunk(int startChunk, int endChunk)
{
    FloatSurface& oldSurface = OtherSurface();

    for (int y = startChunk >= 1 ? startChunk : 1; y < endChunk; y++)
    {
        for (int x = 1; x < w - 1; x++)
        {
            CurrentSurface().XYValue(x, y) = (oldSurface.XYValue(x - 1, y - 1) +
                                              oldSurface.XYValue(x    , y - 1) +
                                              oldSurface.XYValue(x + 1, y - 1) +

                                              oldSurface.XYValue(x - 1, y    ) +
                                              oldSurface.XYValue(x + 1, y    ) +

                                              oldSurface.XYValue(x - 1, y + 1) +
                                              oldSurface.XYValue(x    , y + 1) +
                                              oldSurface.XYValue(x + 1, y + 1)
                                              ) / 8.0f;
        }
    }
}

float Simulator::SlowValueAverager(FloatSurface& surface, int x, int y)
{
    float total = 0;
    int n = 0;
    if (y > 0)
    {
        total += surface.XYValue(x, y - 1);
        n++;
        if (x < w - 2)
        {
            total += surface.XYValue(x + 1, y - 1);
            n++;
        }
    }

    if (x > 0)
    {
        total += surface.XYValue(x - 1, y);
        n++;
        if (y < h - 2)
        {
            total += surface.XYValue(x - 1, y + 1);
            n++;
        }
        if (y > 0)
        {
            total += surface.XYValue(x - 1, y - 1);
            n++;
        }
    }

    if (x < w - 2)
    {
        total += surface.XYValue(x + 1, y    );
        n++;
        if (y < h - 2)
        {
            total += surface.XYValue(x + 1, y + 1);
            n++;
        }
    }

    if (y < h - 2)
    {
        total += surface.XYValue(x, y + 1);
        n++;
    }


    return total / (float) n;
}


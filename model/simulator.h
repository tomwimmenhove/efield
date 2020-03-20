#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <functional>
#include <QMutex>

#include "surface.h"

class Simulator
{
public:
    Simulator(int Width, int Height, std::function<void(Surface&)> updateBoundaries);

    inline int Height() const { return h; }
    inline int Width() const { return w; }

    inline Surface& CurrentSurface() { return surfaces[curBufIdx]; }

    QSharedPointer<Surface> CloneSurface();

    void IterateSimulation();

    inline int NumChunks() const { return h; }
    void PreIterateSimulationChunk();
    void IterateSimulationChunk(int startChunk, int endChunk);
    void PostIterateSimulationChunk() { updateBoundaries(CurrentSurface()); }

private:
    inline Surface& OtherSurface() { return surfaces[curBufIdx ^ 1]; }
    void SwitchSurface();

    float SlowValueAverager(Surface& surface, int x, int y);

    int w, h;
    std::function<void(Surface&)> updateBoundaries;
    int curBufIdx = 0;

    Surface surfaces[2] = { Surface(w, h, 0), Surface(w, h, 0), };
    QMutex surfaceMutexex[2];
};

#endif // SIMULATOR_H

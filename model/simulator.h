#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <functional>
#include <QMutex>

#include "surface.h"
#include "floatsurface.h"

class Simulator
{
public:
    Simulator(int width, int height, std::function<void(FloatSurface&)> updateBoundaries);

    inline int Height() const { return h; }
    inline int Width() const { return w; }

    inline FloatSurface& CurrentSurface() { return surfaces[curBufIdx]; }

    QSharedPointer<FloatSurface> CloneSurface();

    void IterateSimulation();

    inline int NumChunks() const { return h; }
    void PreIterateSimulationChunk();
    void IterateSimulationChunk(int startChunk, int endChunk);
    void PostIterateSimulationChunk() { updateBoundaries(CurrentSurface()); }

private:
    inline FloatSurface& OtherSurface() { return surfaces[curBufIdx ^ 1]; }
    void SwitchSurface();

    float SlowValueAverager(FloatSurface& surface, int x, int y);

    int w, h;
    std::function<void(FloatSurface&)> updateBoundaries;
    int curBufIdx = 0;

    FloatSurface surfaces[2] = { FloatSurface(w, h, 0), FloatSurface(w, h, 0), };
    QMutex surfaceMutexex[2];
};

#endif // SIMULATOR_H

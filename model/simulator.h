#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <functional>

#include "surface.h"

class Simulator
{
public:
    Simulator(int Width, int Height, std::function<void(Surface*)> updateBoundaries);

    inline int Height() const { return h; }
    inline int Width() const { return w; }

    inline Surface& CurrentSurface() { return surfaces[curBufIdx]; }

    void IterateSimulation();

    inline int NumChunks() const { return h; }
    void PreIterateSimulationChunk();
    void IterateSimulationChunk(int startChunk, int endChunk);
    void PostIterateSimulationChunk() { updateBoundaries(curSurface); }

private:
    inline Surface* OtherSurface() { return &surfaces[curBufIdx ^ 1]; }
    inline Surface* SwitchSurface() { return curSurface = &surfaces[curBufIdx ^= 1]; }

    float SlowValueAverager(Surface* surface, int x, int y);

    int w, h;
    std::function<void(Surface*)> updateBoundaries;
    int curBufIdx = 0;

    Surface* curSurface;
    Surface surfaces[2] = { Surface(w, h, 0), Surface(w, h, 0), };
};

#endif // SIMULATOR_H

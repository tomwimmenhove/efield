#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "surface.h"

class Simulator
{
public:
    Simulator(int Width, int Height);

    inline int Height() const { return h; }
    inline int Width() const { return w; }

    inline Surface& CurrentSurface() { return surfaces[curBufIdx]; }

    void IterateSimulation();

private:
    void SwitchBuf();
    float SlowValueAverager(Surface* surface, int x, int y);

    int w, h;
    int curBufIdx = 0;

    Surface* curSurface;
    Surface surfaces[2] = { Surface(w, h, 0), Surface(w, h, 0), };
};

#endif // SIMULATOR_H

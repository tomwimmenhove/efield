#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <functional>
#include <QMutex>

#include "surface.h"
#include "floatsurface.h"

class Simulator
{
public:
    Simulator()
    { }

    Simulator(const QSize& size, std::function<void(FloatSurface&)> updateBoundariesHandler);

    inline QSize size() const { return QSize(w, h); }
    inline FloatSurface& currentSurface() { return surfaces[curBufIdx]; }
    QSharedPointer<FloatSurface> cloneSurface();

    void iterateSimulation();

    inline int numChunks() const { return h; }
    void preIterateSimulationChunk();
    void iterateSimulationChunk(int startChunk, int endChunk);
    inline void postIterateSimulationChunk() { updateBoundariesHandler(currentSurface()); }

private:
    inline FloatSurface& otherSurface() { return surfaces[curBufIdx ^ 1]; }
    void switchSurface();

    float slowValueAverager(FloatSurface& surface, int x, int y);

    int w, h;
    std::function<void(FloatSurface&)> updateBoundariesHandler;
    int curBufIdx = 0;

    FloatSurface surfaces[2] = { FloatSurface(w, h, 0), FloatSurface(w, h, 0), };
    QMutex surfaceMutexex[2];
};

#endif // SIMULATOR_H

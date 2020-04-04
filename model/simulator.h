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

    inline QSize Size() const { return QSize(w, h); }
    inline void SetSize(const QSize& size) { w = size.width(); h = size.height(); }

    inline FloatSurface& CurrentSurface() { return surfaces[curBufIdx]; }

    QSharedPointer<FloatSurface> CloneSurface();

    void IterateSimulation();

    inline int NumChunks() const { return h; }
    void PreIterateSimulationChunk();
    void IterateSimulationChunk(int startChunk, int endChunk);
    inline void PostIterateSimulationChunk() { updateBoundariesHandler(CurrentSurface()); }

private:
    inline FloatSurface& OtherSurface() { return surfaces[curBufIdx ^ 1]; }
    void SwitchSurface();

    float SlowValueAverager(FloatSurface& surface, int x, int y);

    int w, h;
    std::function<void(FloatSurface&)> updateBoundariesHandler;
    int curBufIdx = 0;

    FloatSurface surfaces[2] = { FloatSurface(w, h, 0), FloatSurface(w, h, 0), };
    QMutex surfaceMutexex[2];
};

#endif // SIMULATOR_H

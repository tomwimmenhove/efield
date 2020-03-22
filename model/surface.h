#ifndef SURFACE_H
#define SURFACE_H

#include <QSharedPointer>
#include <QVector2D>
#include <vector>
#include <limits>

#include "surface.h"
#include "drawing.h"

template<typename T>
class Surface
{
public:
    Surface(int width, int height, T initValue)
        : Surface(width, height)
    {
        for (int i = 0; i < w * h ;i++)
            values[i] = initValue;
    }

    inline T& XYValue(int x, int y) { return values[x + y * w]; }
    inline T XYCValue(int x, int y) const { return values[x + y * w]; }
    inline int Height() const { return h; }
    inline int Width() const { return w; }

protected:
    inline Surface(int width, int height) : w(width), h(height), values(width * height) { }

    int w, h;
    std::vector<T> values; // BUG: QVector is ~3x slower!
};

#endif // SURFACE_H

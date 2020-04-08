#ifndef SURFACE_H
#define SURFACE_H

#include <QSharedPointer>
#include <QVector2D>
#include <QSize>
#include <vector>
#include <limits>

#include "surface.h"
#include "graphics/drawing.h"

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

    inline T value(int x, int y) const { return values[x + y * w]; }
    inline void setValue(int x, int y, T value) { values[x + y * w] = value; }

    inline int height() const { return h; }
    inline int width() const { return w; }
    inline QSize size() const { return QSize(w, h); }

protected:
    inline Surface(int width, int height) : w(width), h(height), values(width * height) { }
    inline Surface(QSize size) : Surface(size.width(), size.height()) { }

    int w, h;
    std::vector<T> values; // BUG: QVector is ~3x slower!
};

#endif // SURFACE_H

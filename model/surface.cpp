#include <limits>

#include "surface.h"

Surface::Surface(int width, int height)
    : w(width), h(height)
{
    values = new float[w * h];
}

Surface::Surface(int width, int height, float initValue)
    : Surface(width, height)
{
    for (int i = 0; i < w * h ;i++)
    {
        values[i] = initValue;
    }
}

Surface::~Surface()
{
    delete[] values;
}

float Surface::MaxValue() const
{
    float max = std::numeric_limits<int>::min();

    for (int i = 0; i < w * h; i++)
    {
        float f = values[i];
        if (f > max)
            max = f;
    }

    return max;
}

float Surface::MinValue() const
{
    float min = std::numeric_limits<int>::max();

    for (int i = 0; i < w * h; i++)
    {
        float f = values[i];
        if (f < min)
            min = f;
    }

    return min;
}

QSharedPointer<Surface> Surface::Clone()
{
    Surface* s = new Surface(w, h);

    for (int i = 0; i < w * h; i++)
    {
        s->values[i] = values[i];
    }

    return QSharedPointer<Surface>(s);
}

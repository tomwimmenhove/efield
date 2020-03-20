#include <limits>

#include "surface.h"

Surface::Surface(int width, int height, float initValue)
    : Surface(width, height)
{
    for (int i = 0; i < w * h ;i++)
        values[i] = initValue;
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

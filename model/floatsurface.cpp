#include "floatsurface.h"
#include "gradientsurface.h"

FloatSurface::FloatSurface(const GradientSurface& gradientSurface)
    : FloatSurface(gradientSurface.Width(), gradientSurface.Height())
{
    for (int y = 0; y < this->h; y++)
    {
        for (int x = 0;x  < this->w; x++)
        {
            XYValue(x, y) = gradientSurface.XYCValue(x, y).length();
        }
    }
}

float FloatSurface::MaxValue() const
{
    float max = std::numeric_limits<float>::min();

    for (int i = 0; i < this->w * this->h; i++)
    {
        float f = this->values[i];
        if (f > max)
            max = f;
    }

    return max;
}

float FloatSurface::MinValue() const
{
    float min = std::numeric_limits<float>::max();

    for (int i = 0; i < this->w * this->h; i++)
    {
        float f = this->values[i];
        if (f < min)
            min = f;
    }

    return min;
}

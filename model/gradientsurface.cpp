#include "gradientsurface.h"
#include "floatsurface.h"

GradientSurface::GradientSurface(const FloatSurface& floatSurface)
    : Surface<QVector2D>(floatSurface.width() - 1, floatSurface.height() - 1)
{
    for (int y = 0; y < this->h; y++)
    {
        for (int x = 0;x  < this->w; x++)
        {
            setValue(x, y, getVector(floatSurface, x, y));
        }
    }
}

float GradientSurface::maxLength() const
{
    float max = std::numeric_limits<float>::min();

    for (int i = 0; i < this->w * this->h; i++)
    {
        float f = this->values[i].length();
        if (f > max)
            max = f;
    }

    return max;
}

float GradientSurface::minLength() const
{
    float min = std::numeric_limits<float>::max();

    for (int i = 0; i < this->w * this->h; i++)
    {
        float f = this->values[i].length();
        if (f < min)
            min = f;
    }

    return min;
}

QVector2D GradientSurface::getVector(const FloatSurface& floatSurface, int x, int y) const
{
    float x0y0 = floatSurface.value(x + 0, y + 0);
    float x1y0 = floatSurface.value(x + 1, y + 0);
    float x0y1 = floatSurface.value(x + 0, y + 1);
    float x1y1 = floatSurface.value(x + 1, y + 1);

    return QVector2D(((x1y1 - x0y1) + (x1y0 - x0y0)) / 2,
                     ((x0y0 - x0y1) + (x1y0 - x1y1)) / 2);
}

#ifndef FLOATSURFACE_H
#define FLOATSURFACE_H

#include "surface.h"

class GradientSurface;

class FloatSurface : public Surface<float>
{
public:
    inline FloatSurface(int width, int height, float initValue) : Surface<float>(width, height, initValue) { }
    FloatSurface(const GradientSurface& gradientSurface);

    float maxValue() const;
    float minValue() const;

private:
    inline FloatSurface(int width, int height) : Surface<float>(width, height) { }
};

#endif // FLOATSURFACE_H

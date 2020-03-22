#ifndef GRADIENTSURFACE_H
#define GRADIENTSURFACE_H

#include <QVector2D>
#include <vector>
#include <limits>

#include "surface.h"

class FloatSurface;

class GradientSurface : public Surface<QVector2D>
{
public:
    GradientSurface(const FloatSurface& floatSurface);

    float MaxLength() const;
    float MinLength() const;

private:
    QVector2D GetVector(const FloatSurface& floatSurface, int x, int y) const;
};

#endif // GRADIENTSURFACE_H

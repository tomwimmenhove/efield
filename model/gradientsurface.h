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

    float maxLength() const;
    float minLength() const;

private:
    QVector2D getVector(const FloatSurface& floatSurface, int x, int y) const;
};

#endif // GRADIENTSURFACE_H

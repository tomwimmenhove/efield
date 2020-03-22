#ifndef FLOATSURFACEDRAWER_H
#define FLOATSURFACEDRAWER_H

#include "surface.h"
#include "idrawer.h"

class FloatSurfaceDrawer : public IDrawer<float>
{
public:
    FloatSurfaceDrawer(FloatSurface& surface)
     : surface(surface)
    { }

    void PutPixel(int x, int y, float value) override { surface.XYValue(x, y) = value; }

private:
    FloatSurface& surface;
};

#endif // FLOATSURFACEDRAWER_H

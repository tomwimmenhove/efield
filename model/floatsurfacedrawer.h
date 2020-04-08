#ifndef FLOATSURFACEDRAWER_H
#define FLOATSURFACEDRAWER_H

#include "floatsurface.h"
#include "graphics/drawing.h"

class FloatSurfaceDrawer : public IDrawer<float>
{
public:
    FloatSurfaceDrawer(FloatSurface& surface)
     : surface(surface)
    { }

    void putPixel(int x, int y, float value) override { surface.setValue(x, y, value); }

private:
    FloatSurface& surface;
};

#endif // FLOATSURFACEDRAWER_H

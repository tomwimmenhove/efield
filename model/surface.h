#ifndef SURFACE_H
#define SURFACE_H

#include <QSharedPointer>

class Surface
{
public:
    Surface(int width, int height, float initValue);
    ~Surface();

    inline float& XYValue(int x, int y) { return values[x + y * w]; }
    inline int Height() const { return h; }
    inline int Width() const { return w; }

    void IterateSimulation();
    float MaxValue() const;
    float MinValue() const;

    QSharedPointer<Surface> Clone();

private:
    Surface(int width, int height);

    int w, h;
    float* values;
};

#endif // SURFACE_H

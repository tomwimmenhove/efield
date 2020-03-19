#ifndef SURFACE_H
#define SURFACE_H

#include <QSharedPointer>
#include <QVector>

class Surface
{
public:
    Surface(int width, int height, float initValue);

    inline float& XYValue(int x, int y) { return values[x + y * w]; }
    inline int Height() const { return h; }
    inline int Width() const { return w; }

    void IterateSimulation();
    float MaxValue() const;
    float MinValue() const;

    QSharedPointer<Surface> Clone();

private:
    inline Surface(int width, int height) : w(width), h(height), values(width * height) { }

    int w, h;
    QVector<float> values;
};

#endif // SURFACE_H

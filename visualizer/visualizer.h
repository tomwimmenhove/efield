#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <QImage>
#include <QPixmap>
#include <QPainter>

#include <model/floatsurface.h>
#include <model/gradientsurface.h>
#include <util/ivaluestepper.h>

class Visualizer
{
public:
    static QImage QImageFromFloatSurface(const FloatSurface& surface, const IValueStepper& stepper);
    static void PaintGradientVectors(QPainter& painter, const GradientSurface& gradientSurface, int spacing);

private:
    static QPixmap MakeArrow();
};

#endif // VISUALIZER_H

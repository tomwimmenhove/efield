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
    static QImage imageFromFloatSurface(const FloatSurface& surface, const IValueStepper& stepper);
    static void paintGradientVectors(QPainter& painter, const GradientSurface& gradientSurface, int spacing);

private:
    static QPixmap makeArrow();
};

#endif // VISUALIZER_H

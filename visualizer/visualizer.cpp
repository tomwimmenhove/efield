#include <cmath>

#include "visualizer.h"

#include "util/simplevaluestepper.h"
#include "view/heatmap.h"

QImage Visualizer::imageFromFloatSurface(const FloatSurface& surface, const IValueStepper& stepper)
{
    int w = surface.width();
    int h = surface.height();
    float max = surface.maxValue();
    float min = surface.minValue();
    float range = max - min;

    QImage image(w, h, QImage::Format_ARGB32);
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            /* Scale between  0..1 */
            if (range != 0)
            {
                float value = surface.value(x, y);
                float stepped = stepper.makeStepped(value);
                float f = (stepped - min) / range;

                QRgb color = HeatMap::getColor(f, 0);
                image.setPixel(x, (h - y - 1), color);
            }
        }
    }

    return image;
}

void Visualizer::paintGradientVectors(QPainter& painter, const GradientSurface& gradientSurface, int spacing)
{
    static QPixmap arrow = makeArrow();

    int surfaceWidth = gradientSurface.width();
    int surfaceHeight = gradientSurface.height();
    int painterWidth = painter.device()->width();
    int painterHeight = painter.device()->height();

    for (int y = 0; y < painterHeight; y += spacing)
    {
        for (int x = 0; x < painterWidth; x += spacing)
        {
            int nx = x * surfaceWidth / painterWidth;
            int ny = (painterHeight - 1 - y) * surfaceHeight / painterHeight;

            QVector2D v = -gradientSurface.value(nx, ny); // Negated, because E-field lines go from positive to negative
            float a = atan2(v.y(), v.x());
            QMatrix rm;
            rm.rotate(a * 180 / M_PI);

            painter.drawPixmap(x - arrow.width(), y - arrow.height(), arrow.transformed(rm));
        }
    }
}

QPixmap Visualizer::makeArrow()
{
    QPixmap arrow = QPixmap(9, 9);
    arrow.fill(QColor(0, 0, 0, 255));
    arrow.fill(QColor(0, 0, 0, 0));
    QPainter arrowPainter(&arrow);
    //arrowPainter.setRenderHint(QPainter::Antialiasing);
    arrowPainter.drawLine(0, 4, 8, 4);
    arrowPainter.drawLine(4, 0, 8, 4);
    arrowPainter.drawLine(4, 8, 8, 4);

    return arrow;
}

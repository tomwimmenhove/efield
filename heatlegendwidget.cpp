#include "heatmap.h"
#include "heatlegendwidget.h"

#include <cmath>

#include <QPainter>
#include <QImage>
#include <QDebug>

HeatLegendWidget::HeatLegendWidget(QWidget *parent)
    : QWidget(parent)
{ }

void HeatLegendWidget::SetMin(double value)
{
    min = value;

    UpdateTickStep();

    update();
}

void HeatLegendWidget::SetMax(double value)
{
    max = value;

    UpdateTickStep();

    update();
}

double HeatLegendWidget::NiceNumber (double value, bool round)
{
    double niceFraction;

    int exponent = (int) floor(log10(value));
    double fraction = value/pow(10, (double)exponent);

    if (round) {
        if (fraction < 1.5)
            niceFraction = 1.0;
        else if (fraction < 3.0)
            niceFraction = 2.0;
        else if (fraction < 7.0)
            niceFraction = 5.0;
        else
            niceFraction = 10.0;
    }
    else {
        if (fraction <= 1.0)
            niceFraction = 1.0;
        else if (fraction <= 2.0)
            niceFraction = 2.0;
        else if (fraction <= 5.0)
            niceFraction = 5.0;
        else
            niceFraction = 10.0;
    }

    return niceFraction*pow(10, (double)exponent);
}

double HeatLegendWidget::GetNiceTicks(double& axisStart, double& axisEnd, int maxTicks)
{
      /* Check for special cases */
    double axisWidth = axisEnd - axisStart;
    if (axisWidth == 0.0)
        return 0;

      /* Compute the new nice range and ticks */
    double nceRange = NiceNumber(axisEnd - axisStart, 0);
    double niceTick = NiceNumber(nceRange / ((float) maxTicks - 1), 1);

      /* Compute the new nice start and end values */
    axisStart = floor(axisStart / niceTick) * niceTick;
    axisEnd = ceil(axisEnd / niceTick) * niceTick;

    return niceTick;
}

void HeatLegendWidget::UpdateTickStep()
{
    axisStart = min;
    axisEnd = max;
    tickStep = GetNiceTicks(axisStart, axisEnd, maxTicks);
}

void HeatLegendWidget::setStepped(bool value)
{
    stepped = value;

    update();
}

float HeatLegendWidget::GetSteppedValue(float value)
{
    if (stepped)
        return (qRound(value / tickStep)) * tickStep;

    return value;
}

void HeatLegendWidget::paintEvent(QPaintEvent*)
{
    int h = height();
    int barWidth = 20;

    std::vector<QRgb> pixels(barWidth * h);
    QPainter painter(this);

    int textHeight = painter.fontMetrics().height();
    int barHeight = h - textHeight;
    int barTop = h - textHeight / 2 - barHeight;
    int barBottom = h - textHeight / 2 - 1;

    /* Calculate the ticks */

    UpdateTickStep();

    if (tickStep == 0)
        return;

    /* Draw the ticks */
    int n = 0;
    int lastY = std::numeric_limits<int>::max();
    for (double l = axisStart; l <= axisEnd && l <= max; ++n, l = axisStart + (double) n * tickStep)
    {
        if (l < min)
            continue;

        /* Interpolate pixel-position */
        int y = (int) qRound((float) barBottom + (l - min) * ((float) barTop - (float) barBottom) / (max - min));

        /* Draw the tick line */
        painter.drawLine(barWidth, y, barWidth + 5, y);
        painter.drawLine(barWidth + 3, y - 2, barWidth + 5, y);
        painter.drawLine(barWidth + 3, y + 2, barWidth + 5, y);

        /* Make sure the tick-strings don't get too close */
        if (lastY - y > textHeight)
        {
            QString tickNumberString = QString::number(l, 'g', 3);

            int yCenter = painter.fontMetrics().tightBoundingRect(tickNumberString).center().y();
            painter.drawText(barWidth + 10, y - yCenter, tickNumberString);

            lastY = y;
        }
    }

    /* Draw the color-bar */
    QImage image = QImage((uchar*)pixels.data(), barWidth, h, QImage::Format_ARGB32);
    for (int y = 0; y < barHeight; ++y)
    {
        /* Convert pixel value to actual value */
        float f = ((float) y / barHeight) * (max - min) + min;

        f = GetSteppedValue(f);

        /* Convert actual value to a normalized (0..1) value */
        float fNorm = (f - min) / (max - min);

        /* Get the color for this value */
        QRgb color = HeatMap::GetColor(fNorm);

        for (int x = 0; x < barWidth; ++x)
            pixels[x + (barBottom - y) * barWidth] = color;
    }
    painter.drawImage(0, 0, image);
}

#include "heatmap.h"
#include "heatlegendwidget.h"

#include <cmath>

#include <QPainter>
#include <QImage>
#include <QDebug>

HeatLegendWidget::HeatLegendWidget(QWidget *parent)
    : QWidget(parent)
{ }

void HeatLegendWidget::setMin(double value)
{
    minVal = value;

    updateTickStepu();

    update();
}

void HeatLegendWidget::setMax(double value)
{
    maxVal = value;

    updateTickStepu();

    update();
}

double HeatLegendWidget::niceNumber (double value, bool round)
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

double HeatLegendWidget::getNiceTicks(double& axisStart, double& axisEnd, int maxTicks)
{
      /* Check for special cases */
    double axisWidth = axisEnd - axisStart;
    if (axisWidth == 0.0)
        return 0;

      /* Compute the new nice range and ticks */
    double nceRange = niceNumber(axisEnd - axisStart, 0);
    double niceTick = niceNumber(nceRange / ((float) maxTicks - 1), 1);

      /* Compute the new nice start and end values */
    axisStart = floor(axisStart / niceTick) * niceTick;
    axisEnd = ceil(axisEnd / niceTick) * niceTick;

    return niceTick;
}

void HeatLegendWidget::updateTickStepu()
{
    axisStart = minVal;
    axisEnd = maxVal;
    tickStepVal = getNiceTicks(axisStart, axisEnd, maxTicks);
}

void HeatLegendWidget::setStepped(bool value)
{
    stepped = value;

    update();
}

float HeatLegendWidget::steppedValue(float value)
{
    if (stepped)
        return (qRound(value / tickStepVal)) * tickStepVal;

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

    updateTickStepu();

    if (tickStepVal == 0)
        return;

    /* Draw the ticks */
    int n = 0;
    int lastY = std::numeric_limits<int>::max();
    for (double l = axisStart; l <= axisEnd && l <= maxVal; ++n, l = axisStart + (double) n * tickStepVal)
    {
        if (l < minVal)
            continue;

        /* Interpolate pixel-position */
        int y = (int) qRound((float) barBottom + (l - minVal) * ((float) barTop - (float) barBottom) / (maxVal - minVal));

        /* Draw the tick arrow */
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
        float f = ((float) y / barHeight) * (maxVal - minVal) + minVal;

        f = steppedValue(f);

        /* Convert actual value to a normalized (0..1) value */
        float fNorm = (f - minVal) / (maxVal - minVal);

        /* Get the color for this value */
        QRgb color = HeatMap::getColor(fNorm);

        for (int x = 0; x < barWidth; ++x)
            pixels[x + (barBottom - y) * barWidth] = color;
    }
    painter.drawImage(0, 0, image);
}

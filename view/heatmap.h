#ifndef HEATMAP_H
#define HEATMAP_H

#include <QRgb>

class HeatMap
{
public:
    static QRgb getColor(float value, int steps = 0);
};

#endif // HEATMAP_H

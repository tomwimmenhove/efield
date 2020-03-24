#ifndef HEATMAP_H
#define HEATMAP_H

#include <QRgb>

class HeatMap
{
public:
    static QRgb GetColor(float value, int steps);
};

#endif // HEATMAP_H

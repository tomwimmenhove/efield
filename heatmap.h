#ifndef HEATMAP_H
#define HEATMAP_H

#include <QRgb>

class HeatMap
{
public:
    static QRgb GetColor(float value);
};

#endif // HEATMAP_H

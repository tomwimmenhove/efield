#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <QVector2D>
#include <QPoint>
#include <QSize>

class Geometry
{
public:
    static float DistanceSquared(const QVector2D& v, const QVector2D& w);
    static float Distance(const QVector2D& v, const QVector2D& w);

    static QPoint ScalePoint(const QPoint& point, const QSize& fromSize, const QSize& toSize);

    // Thanks to https://stackoverflow.com/questions/849211/shortest-distance-between-a-point-and-a-line-segment
    static float DistToLine(const QVector2D& v, const QVector2D& w, const QVector2D& p);

    static QPoint TranslatePoint(const QPoint& point,
                                 const QSize& fromSurface,
                                 const QSize& toSurface,
                                 bool fipVertical,
                                 Qt::AspectRatioMode mode = Qt::KeepAspectRatio);
};

#endif // GEOMETRY_H

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <QVector2D>
#include <QPoint>
#include <QSize>

class Geometry
{
public:
    static float distanceSquared(const QVector2D& v, const QVector2D& w);
    static float distance(const QVector2D& v, const QVector2D& w);

    static QPoint scalePoint(const QPoint& point, const QSize& fromSize, const QSize& toSize);

    // Thanks to https://stackoverflow.com/questions/849211/shortest-distance-between-a-point-and-a-line-segment
    static float distToLine(const QVector2D& v, const QVector2D& w, const QVector2D& p);

    static QPoint translatePoint(const QPoint& point,
                                 const QSize& fromSurface,
                                 const QSize& toSurface,
                                 bool fipVertical,
                                 Qt::AspectRatioMode mode = Qt::KeepAspectRatio);
};

#endif // GEOMETRY_H

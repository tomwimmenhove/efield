#include <QtGlobal>
#include <cmath>

#include "geometry.h"

float Geometry::DistanceSquared(const QVector2D& v, const QVector2D& w)
{
    QVector2D d = v - w;
    return d.x() * d.x() + d.y() * d.y();
}

float Geometry::Distance(const QVector2D& v, const QVector2D& w)
{
    return sqrt(DistanceSquared(v, w));
}

QPoint Geometry::ScalePoint(const QPoint& point, const QSize& fromSize, const QSize& toSize)
{
    return QPoint(point.x() * toSize.width() / fromSize.width(),
                  point.y() * toSize.height() / fromSize.height());
}

float Geometry::DistToLine(const QVector2D& v, const QVector2D& w, const QVector2D& p)
{
    // Return minimum distance between line segment vw and point p
    float l2 = DistanceSquared(w, v);  // i.e. |w-v|^2 -  avoid a sqrt
    if (l2 == 0.0) return Distance(p, v);   // v == w case
    // Consider the line extending the segment, parameterized as v + t (w - v).
    // We find projection of point p onto the line.
    // It falls where t = [(p-v) . (w-v)] / |w-v|^2
    // We clamp t from [0,1] to handle points outside the segment vw.

    float t = qMax(0.0f, qMin(1.0f, QVector2D::dotProduct(p - v, w - v) / l2));
    QVector2D projection = v + t * (w - v);  // Projection falls on the segment

    return Distance(p, projection);
}

QPoint Geometry::TranslatePoint(const QPoint& point, const QSize& fromSurface, const QSize& toSurface, bool fipVertical, Qt::AspectRatioMode mode)
{
    QSize scaled = toSurface.scaled(fromSurface, mode);
    QSize offset = (fromSurface - scaled) / 2;
    QPoint translated = Geometry::ScalePoint(point - QPoint(offset.width(), offset.height()), scaled, toSurface);

    if (fipVertical)
        translated.setY(toSurface.height() - 1 - translated.y());

    return translated;
}

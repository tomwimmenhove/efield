#include <QtGlobal>
#include <cmath>

#include "geometry.h"

float Geometry::distanceSquared(const QVector2D& v, const QVector2D& w)
{
    QVector2D d = v - w;
    return d.x() * d.x() + d.y() * d.y();
}

float Geometry::distance(const QVector2D& v, const QVector2D& w)
{
    return sqrt(distanceSquared(v, w));
}

QPoint Geometry::scalePoint(const QPoint& point, const QSize& fromSize, const QSize& toSize)
{
    return QPoint(point.x() * toSize.width() / fromSize.width(),
                  point.y() * toSize.height() / fromSize.height());
}

QSize Geometry::scaleSize(const QSize& size, const QSize& fromSize, const QSize& toSize)
{
    return QSize(size.width() * toSize.width() / fromSize.width(),
                  size.height() * toSize.height() / fromSize.height());
}

float Geometry::distToLine(const QVector2D& v, const QVector2D& w, const QVector2D& p)
{
    // Return minimum distance between line segment vw and point p
    float l2 = distanceSquared(w, v);  // i.e. |w-v|^2 -  avoid a sqrt
    if (l2 == 0.0) return distance(p, v);   // v == w case
    // Consider the line extending the segment, parameterized as v + t (w - v).
    // We find projection of point p onto the line.
    // It falls where t = [(p-v) . (w-v)] / |w-v|^2
    // We clamp t from [0,1] to handle points outside the segment vw.

    float t = qMax(0.0f, qMin(1.0f, QVector2D::dotProduct(p - v, w - v) / l2));
    QVector2D projection = v + t * (w - v);  // Projection falls on the segment

    return distance(p, projection);
}

QPoint Geometry::translatePoint(const QPoint& point, const QSize& fromSurface, const QSize& toSurface, bool fipVertical, Qt::AspectRatioMode mode)
{
    QSize scaled = toSurface.scaled(fromSurface, mode);
    QSize offset = (fromSurface - scaled) / 2;
    QPoint translated = Geometry::scalePoint(point - QPoint(offset.width(), offset.height()), scaled, toSurface);

    if (fipVertical)
        translated.setY(toSurface.height() - 1 - translated.y());

    return translated;
}

QPoint Geometry::clip(QPoint p, const QSize& size)
{
    if (p.x() < 0) p.setX(0);
    if (p.y() < 0) p.setY(0);
    if (p.x() >= size.width()) p.setX(size.width() - 1);
    if (p.y() >= size.height()) p.setY(size.height() - 1);

    return p;
}

QRect Geometry::clip(QRect r, const QRect& bounds)
{
    if (r.left() < bounds.left()) r.translate(bounds.left() - r.left(), 0);
    if (r.right() < bounds.left()) r.translate(bounds.left() - r.right(), 0);

    if (r.right() > bounds.right()) r.translate(bounds.right() - r.right(), 0);
    if (r.left() > bounds.right()) r.translate(bounds.right() - r.left(), 0);


    if (r.top() < bounds.top()) r.translate(0, bounds.top() - r.top());
    if (r.bottom() < bounds.top()) r.translate(0, bounds.top() - r.bottom());

    if (r.bottom() > bounds.bottom()) r.translate(0, bounds.bottom() - r.bottom());
    if (r.top() > bounds.bottom()) r.translate(0, bounds.bottom() - r.top());


    return r;
}

QRect Geometry::clip(QRect r, const QSize& size)
{
    return clip(r, QRect(QPoint(0, 0), size));
}

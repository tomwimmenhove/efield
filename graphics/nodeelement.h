#ifndef NODEELEMENT_H
#define NODEELEMENT_H

#include "sharednode.h"
#include "drawingelement.h"
#include "graphics/drawing.h"
#include "util/geometry.h"

template<typename T>
class NodeElement : public DrawingElement<T>
{
public:
    NodeElement(const SharedNode& p, int margin = 3)
        : p(p), margin(margin)
    { }

    static QSharedPointer<DrawingElement<float>> SharedElement(const SharedNode& p, int margin = 3)
    {
        return  QSharedPointer<DrawingElement<float>>(new NodeElement<float>(p, margin));
    }

    void Draw(IDrawer<T>&) override { } /* Annotation only */

    void DrawAnnotation(QPainter& painter, const QSize& surfaceSize) override
    {
        QPoint sp = Geometry::ScalePoint(p, surfaceSize, QSize(painter.device()->width(), painter.device()->height()));

        int y = painter.device()->height() - 1 - sp.y();

        if (this->IsHighlighted())
            painter.setPen(Qt::red);
        else
            painter.setPen(Qt::black);

        painter.drawLine(sp.x() - margin, y - margin, sp.x() + margin, y - margin);
        painter.drawLine(sp.x() - margin, y - margin, sp.x() - margin, y + margin);
        painter.drawLine(sp.x() - margin, y + margin, sp.x() + margin, y + margin);
        painter.drawLine(sp.x() + margin, y - margin, sp.x() + margin, y + margin);
    }

    float DistanceTo(const QPoint& point) const override
    {
        return Geometry::Distance(QVector2D(p), QVector2D(point));
    }

    inline SharedNode Node() const { return p; }

private:
    SharedNode p;
    int margin;
};

#endif // NODEELEMENT_H

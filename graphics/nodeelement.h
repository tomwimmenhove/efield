#ifndef NODEELEMENT_H
#define NODEELEMENT_H

#include <memory>

#include "sharednode.h"
#include "drawingelement.h"
#include "graphics/drawing.h"
#include "util/geometry.h"

template<typename T>
class NodeElement : public DrawingElement<T>
{
public:
    NodeElement(int id, const QSize& bounds)
        : DrawingElement<T>(id, bounds), n(id)
    { }

    NodeElement(const SharedNode& p, const QSize& bounds)
        : DrawingElement<T>(p.identifier(), bounds), n(p)
    { }

    template<typename... Args>
    static std::unique_ptr<NodeElement<T>> uniqueElement(Args&& ...arguments)
    {
        return std::make_unique<NodeElement<T>>(std::forward<Args>(arguments)...);
    }

    void draw(IDrawer<T>&) override { } /* Annotation only */

    void drawAnnotation(QPainter& painter, const QSize& surfaceSize) override
    {
        QPoint sp = Geometry::scalePoint(n, surfaceSize, QSize(painter.device()->width(), painter.device()->height()));

        int y = painter.device()->height() - 1 - sp.y();

        painter.setPen(this->isHighlighted() ? Qt::red : Qt::black);

        painter.drawLine(sp.x() - margin, y - margin, sp.x() + margin, y - margin);
        painter.drawLine(sp.x() - margin, y - margin, sp.x() - margin, y + margin);
        painter.drawLine(sp.x() - margin, y + margin, sp.x() + margin, y + margin);
        painter.drawLine(sp.x() + margin, y - margin, sp.x() + margin, y + margin);
    }

    float distanceTo(const QPoint& point) const override
    {
        float dist = Geometry::distance(QVector2D(n), QVector2D(point)) - margin;
        return dist > 0 ? dist : 0;
    }

    QPoint center() const override { return n.point(); }
    bool setCenter(const QPoint& point) override
    {
        n.setPoint(Geometry::clip(n.point() + point - center(), this->sceneSize()));
        return true;
    }

    QRect bounds() const override { return QRect(center(), QSize(1, 1)); }

    bool canAnchor() const override { return true; }
    bool isInUse() const override { return anchorNode()->refCounter() != 0; }
    QString name() const override { return "Node"; }

    SharedNode anchorNode() const override { return n; }
    inline void setNode(SharedNode node)
    {
        Q_ASSERT(node.identifier() == this->identifier());
        n = node;
    }

    void accept(DrawingElementVisitor<T>& visitor) override { visitor.visit(*this); }

private:
    const int margin = 3;
    SharedNode n;
};

#endif // NODEELEMENT_H

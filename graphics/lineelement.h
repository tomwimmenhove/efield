#ifndef LINEELEMENT_H
#define LINEELEMENT_H

#include <memory>

#include "sharednode.h"
#include "drawingelement.h"
#include "graphics/drawing.h"
#include "util/geometry.h"

template<typename T>
class LineElement : public DrawingElement<T>
{
public:
    LineElement(int id, const QSize& bounds)
        : DrawingElement<T>(id, bounds)
    { }

    LineElement(int id, const QSize& bounds, SharedNode& p1, SharedNode& p2, const T& value)
        : DrawingElement<T>(id, bounds), p1(p1), p2(p2), v(value)
    {
        p1->use();
        p2->use();
    }

    ~LineElement()
    {
        p1->release();
        p2->release();
    }

    template<typename... Args>
    static std::unique_ptr<LineElement<float>> uniqueElement(Args&& ...arguments)
    {
        return std::make_unique<LineElement<float>>(std::forward<Args>(arguments)...);
    }

    virtual drawingElementType elementType() const override { return drawingElementType::Line; }

    void draw(IDrawer<T>& drawer) override
    {
        Drawing<T> drawing(drawer);

        drawing.drawLine(p1, p2, v);
    }

    void drawAnnotation(QPainter& painter, const QSize& surfaceSize) override
    {
        QPoint sp1 = Geometry::scalePoint(p1, surfaceSize, QSize(painter.device()->width(), painter.device()->height()));
        QPoint sp2 = Geometry::scalePoint(p2, surfaceSize, QSize(painter.device()->width(), painter.device()->height()));

        int mh = painter.device()->height() - 1;

        if (this->isHighlighted())
            painter.setPen(Qt::red);
        else
            painter.setPen(Qt::black);

        painter.drawLine(sp1.x(), mh - sp1.y(),
                         sp2.x(), mh - sp2.y());
    }

    float distanceTo(const QPoint& point) const override
    {
        return Geometry::distToLine(QVector2D(p1), QVector2D(p2), QVector2D(point));
    }

    inline T value() const { return v; }
    inline void setValue(const T& value) { v = value; }

    inline SharedNode point1() const { return p1; }
    inline void setPoint1(const SharedNode& value)
    {
        if (p1)
            p1->release();

        p1 = value;
        p1->use();
    }

    inline SharedNode point2() const {return p2; }
    inline void setPoint2(const SharedNode& value)
    {
        if (p2)
            p2->release();

        p2 = value;
        p2->use();
    }

    QPoint center() const override { return (p1.point() + p2.point()) / 2; }
    bool setCenter(const QPoint& point) override
    {
        QPoint d = point - center();
        QPoint newP1 = p1.point() + d;
        QPoint newP2 = p2.point() + d;

        if (!this->isInBounds(newP1) || !this->isInBounds(newP2))
            return false;

        p1.setPoint(newP1);
        p2.setPoint(newP2);

        return true;
    }

    bool canAnchor() const override { return false; }
    bool canDelete() const override { return true; }

    void accept(DrawingElementVisitor<T>& visitor) override { visitor.visit(*this); }

private:
    SharedNode p1;
    SharedNode p2;
    T v;
};

#endif // LINEELEMENT_H

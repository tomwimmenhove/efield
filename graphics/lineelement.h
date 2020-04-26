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

    LineElement (const LineElement&) = delete;

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

    void draw(IDrawer<T>& drawer) override
    {
        Drawing<T> drawing(drawer);

        drawing.drawLine(p1, p2, v);
    }

    void drawAnnotation(QPainter& painter, const QSize& surfaceSize) override
    {
        QSize pSize = QSize(painter.device()->width(), painter.device()->height());
        QPoint sp1 = Geometry::scalePoint(p1, surfaceSize, pSize);
        QPoint sp2 = Geometry::scalePoint(p2, surfaceSize, pSize);

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
        QRect r = Geometry::clip(QRect(p1.point() + d, p2.point() + d), this->sceneBounds());

        p1.setPoint(r.topLeft());
        p2.setPoint(r.bottomRight());

        return true;
    }

    QRect bounds() const override { return QRect(point1().point(), point2().point()); }

    bool canAnchor() const override { return false; }
    bool isInUse() const override { return false; }
    QString name() const override { return "Line"; }

    void accept(DrawingElementVisitor<T>& visitor) override { visitor.visit(*this); }

private:
    SharedNode p1;
    SharedNode p2;
    T v;
};

#endif // LINEELEMENT_H

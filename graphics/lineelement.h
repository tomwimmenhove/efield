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
    LineElement() { }

    LineElement(SharedNode& p1, SharedNode& p2, const T& value)
        : p1(p1), p2(p2), value(value)
    {
        p1->Use();
        p2->Use();
    }

    ~LineElement()
    {
        p1->Release();
        p2->Release();
    }

    static QSharedPointer<LineElement<float>> SharedElement(SharedNode& p1, SharedNode& p2, const T& value)
    {
        return QSharedPointer<LineElement<float>>::create(p1, p2, value);
    }

    static std::unique_ptr<LineElement<float>> UniqueElement(SharedNode& p1, SharedNode& p2, const T& value)
    {
        return std::make_unique<LineElement<float>>(p1, p2, value);
    }

    virtual DrawingElementType ElementType() const { return DrawingElementType::Line; }

    void Draw(IDrawer<T>& drawer) override
    {
        Drawing<T> drawing(drawer);

        drawing.DrawLine(p1, p2, value);
    }

    void DrawAnnotation(QPainter& painter, const QSize& surfaceSize) override
    {
        QPoint sp1 = Geometry::ScalePoint(p1, surfaceSize, QSize(painter.device()->width(), painter.device()->height()));
        QPoint sp2 = Geometry::ScalePoint(p2, surfaceSize, QSize(painter.device()->width(), painter.device()->height()));

        int mh = painter.device()->height() - 1;

        if (this->IsHighlighted())
            painter.setPen(Qt::red);
        else
            painter.setPen(Qt::black);

        painter.drawLine(sp1.x(), mh - sp1.y(),
                         sp2.x(), mh - sp2.y());
    }

    float DistanceTo(const QPoint& point) const override
    {
        return Geometry::DistToLine(QVector2D(p1), QVector2D(p2), QVector2D(point));
    }

    inline T Value() const { return value; }
    inline void SetValue(const T& v) { value = v; }

    inline SharedNode P1() const { return p1; }
    inline void SetP1(const SharedNode& value)
    {
        if (p1)
            p1->Release();

        p1 = value;
        p1->Use();
    }

    inline SharedNode P2() const {return p2; }
    inline void SetP2(const SharedNode& value)
    {
        if (p2)
            p2->Release();

        p2 = value;
        p2->Use();
    }

    void Accept(DrawingElementVisitor<T>& visitor) override { visitor.Visit(*this); }

private:
    SharedNode p1;
    SharedNode p2;
    T value;
};

#endif // LINEELEMENT_H

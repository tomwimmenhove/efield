#ifndef LINEELEMENT_H
#define LINEELEMENT_H

#include "sharednode.h"
#include "drawingelement.h"
#include "graphics/drawing.h"

template<typename T>
class LineElement : public DrawingElement<T>
{
public:
    LineElement(const SharedNode& p1, const SharedNode& p2, const T& value)
        : p1(p1), p2(p2), value(value)
    { }

    static QSharedPointer<DrawingElement<float>> SharedElement(const SharedNode& p1, const SharedNode& p2, const T& value)
    {
        return  QSharedPointer<DrawingElement<float>>(new LineElement<float>(p1, p2, value));
    }

    void Draw(IDrawer<T>& drawer) override
    {
        Drawing<T> drawing(drawer);

        drawing.DrawLine(p1, p2, value);
    }

    void DrawAnnotation(QPainter& painter, const QSize& surfaceSize) override
    {
        int x1 = p1->x() * painter.device()->width() / surfaceSize.width();
        int y1 = painter.device()->height() - 1 - p1->y() * painter.device()->height() / surfaceSize.height();

        int x2 = p2->x() * painter.device()->width() / surfaceSize.width();
        int y2 = painter.device()->height() - 1 - p2->y() * painter.device()->height() / surfaceSize.height();
        painter.drawLine(x1, y1, x2, y2);
    }

private:
    SharedNode p1;
    SharedNode p2;
    T value;
};

#endif // LINEELEMENT_H

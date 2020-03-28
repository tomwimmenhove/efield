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

    void Draw(IDrawer<T>& drawer) override
    {
        Drawing<T> drawing(drawer);

        drawing.DrawLine(p1, p2, value);
    }

    void DrawAnnotation(QPainter& painter, const QSize& graphicsSize, const QSize& surfaceSize) override
    {
        int x1 = p1->x() * graphicsSize.width() / surfaceSize.width();
        int y1 = p1->y() * graphicsSize.height() / surfaceSize.height();

        int x2 = p2->x() * graphicsSize.width() / surfaceSize.width();
        int y2 = p2->y() * graphicsSize.height() / surfaceSize.height();
        painter.drawLine(x1, y1, x2, y2);
    }

private:
    SharedNode p1;
    SharedNode p2;
    T value;
};

#endif // LINEELEMENT_H

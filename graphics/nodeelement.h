#ifndef NODEELEMENT_H
#define NODEELEMENT_H

#include "sharednode.h"
#include "drawingelement.h"
#include "graphics/drawing.h"

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
        int x = p->x() * painter.device()->width() / surfaceSize.width();
        int y = painter.device()->height() - 1 - p->y() * painter.device()->height() / surfaceSize.height();

        painter.drawLine(x - margin, y - margin, x + margin, y - margin);
        painter.drawLine(x - margin, y - margin, x - margin, y + margin);
        painter.drawLine(x - margin, y + margin, x + margin, y + margin);
        painter.drawLine(x + margin, y - margin, x + margin, y + margin);
    }

private:
    SharedNode p;
    int margin;
};

#endif // NODEELEMENT_H

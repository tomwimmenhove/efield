#ifndef DRAWINGELEMENT_H
#define DRAWINGELEMENT_H

#include <QPainter>

#include "graphics/idrawer.h"

template<typename T>
class DrawingElement
{
public:
    virtual void Draw(IDrawer<T>& drawer) = 0;
    virtual void DrawAnnotation(QPainter& painter, const QSize& graphicsSize, const QSize& surfaceSize) = 0;

    virtual ~DrawingElement() { }
};

#endif // DRAWINGELEMENT_H

#ifndef DRAWINGELEMENT_H
#define DRAWINGELEMENT_H

#include <QPoint>
#include <QPainter>

#include "graphics/idrawer.h"

template<typename T>
class DrawingElement
{
public:
    virtual void Draw(IDrawer<T>& drawer) = 0;
    virtual void DrawAnnotation(QPainter& painter, const QSize& surfaceSize) = 0;
    virtual float DistanceTo(const QPoint& point) const = 0;

    virtual ~DrawingElement() { }

    inline bool IsHighlighted() const { return highlighted; }
    inline void SetHighlighted(bool value) { highlighted = value; }

protected:
    bool highlighted = false;
};

#endif // DRAWINGELEMENT_H





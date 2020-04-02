#ifndef DRAWINGELEMENT_H
#define DRAWINGELEMENT_H

#include <QPoint>
#include <QPainter>
#include <QDomDocument>

#include "graphics/idrawer.h"
#include "drawingelementvisitor.h"

enum class DrawingElementType
{
    None = -1,
    Scene,
    Node,
    Line,
};

template<typename T>
class DrawingElement
{
public:
    virtual DrawingElementType ElementType() const = 0;
    virtual void Draw(IDrawer<T>& drawer) = 0;
    virtual void DrawAnnotation(QPainter& painter, const QSize& surfaceSize) = 0;
    virtual float DistanceTo(const QPoint& point) const = 0;

    inline bool IsHighlighted() const { return highlighted; }
    inline void SetHighlighted(bool value) { highlighted = value; }

    virtual void Accept(DrawingElementVisitor<T>& visitor) = 0;

    virtual ~DrawingElement() { }

protected:
    bool highlighted = false;
};

#endif // DRAWINGELEMENT_H





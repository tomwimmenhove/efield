#ifndef DRAWINGELEMENT_H
#define DRAWINGELEMENT_H

#include <QPoint>
#include <QPainter>
#include <QDomDocument>

#include "graphics/idrawer.h"
#include "drawingelementvisitor.h"

enum class drawingElementType
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
    DrawingElement(int id)
        : id(id)
    { }

    virtual drawingElementType elementType() const = 0;
    virtual void draw(IDrawer<T>& drawer) = 0;
    virtual void drawAnnotation(QPainter& painter, const QSize& surfaceSize) = 0;
    virtual float distanceTo(const QPoint& point) const = 0;
    virtual  bool canDelete() const { return true; }

    inline int identifier() const { return id; }

    inline bool isHighlighted() const { return highlighted; }
    inline void setHighlighted(bool value) { highlighted = value; }

    virtual void accept(DrawingElementVisitor<T>& visitor) = 0;

    virtual ~DrawingElement() { }

protected:
    bool highlighted = false;
    int id;
};

#endif // DRAWINGELEMENT_H





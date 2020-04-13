#ifndef DRAWINGELEMENT_H
#define DRAWINGELEMENT_H

#include <QPoint>
#include <QPainter>
#include <QDomDocument>

#include "graphics/idrawer.h"
#include "drawingelementvisitor.h"
#include "sharednode.h"

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
    DrawingElement(int id, const QSize& bounds)
        : id(id), clipBounds(bounds)
    { }

    virtual drawingElementType elementType() const = 0;
    virtual void draw(IDrawer<T>& drawer) = 0;
    virtual void drawAnnotation(QPainter& painter, const QSize& surfaceSize) = 0;
    virtual float distanceTo(const QPoint& point) const = 0;
    virtual QPoint center() const = 0;
    virtual bool setCenter(const QPoint& point) = 0;
    virtual bool canAnchor() const = 0;
    virtual SharedNode anchorNode() const { abort(); }
    virtual bool canDelete() const = 0;

    inline QSize bounds() const { return clipBounds; }
    inline void setBounds(const QSize& bounds) { clipBounds = bounds; }

    inline int identifier() const { return id; }

    inline bool isHighlighted() const { return highlighted; }
    inline void setHighlighted(bool value) { highlighted = value; }

    virtual void accept(DrawingElementVisitor<T>& visitor) = 0;

    virtual ~DrawingElement() { }

protected:
    inline bool isInBounds(const QPoint& p) { return QRect(QPoint(0, 0), clipBounds).contains(p); }

    bool highlighted = false;
    int id;

private:
    QSize clipBounds;
};

#endif // DRAWINGELEMENT_H





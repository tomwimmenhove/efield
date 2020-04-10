#ifndef MOUSEOPERATION_H
#define MOUSEOPERATION_H

#include <memory>
#include <QCursor>
#include <QSharedPointer>

#include "graphics/sceneelement.h"
#include "graphics/lineelement.h"
#include "graphics/nodeelement.h"

class MouseOperation
{
public:
    MouseOperation(std::unique_ptr<MouseOperation>&& parent, QSharedPointer<SceneElement<float>> scene) noexcept
        : parent(std::move(parent)), scene(scene)
    { }

    virtual Qt::CursorShape cursorShape() const { return Qt::ArrowCursor; }

    virtual void activateOperation(std::unique_ptr<MouseOperation>& current, const QPoint& pointerPosition)
    {
        (void) current;
        (void) pointerPosition;
    }

    virtual void cancelOperation(std::unique_ptr<MouseOperation>& current)
    {
        (void) current;
    }

    virtual void mouseMoved(std::unique_ptr<MouseOperation>& current, const QPoint& pointerPosition)
    {
        (void) current;
        (void) pointerPosition;
    }

    virtual void mouseReleased(std::unique_ptr<MouseOperation>& current, const QPoint& pointerPosition, Qt::MouseButtons buttons)
    {
        (void) current;
        (void) pointerPosition;
        (void) buttons;
    }

    virtual void mouseDoubleClicked(std::unique_ptr<MouseOperation>& current, const QPoint& pointerPosition, Qt::MouseButtons buttons)
    {
        (void) current;
        (void) pointerPosition;
        (void) buttons;
    }

    virtual ~MouseOperation() { }

protected:
    std::unique_ptr<MouseOperation> parent;
    QSharedPointer<SceneElement<float>> scene;
};

#endif // MOUSEOPERATION_H

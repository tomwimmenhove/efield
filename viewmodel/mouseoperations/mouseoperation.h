#ifndef MOUSEOPERATION_H
#define MOUSEOPERATION_H

#include <memory>
#include <QCursor>
#include <QSharedPointer>

#include "graphics/sceneelement.h"
#include "util/undo/undostack.h"

class MouseOperation
{
public:
    MouseOperation(std::unique_ptr<MouseOperation>&& parent,
                   const QSharedPointer<UndoStack>& undoStack,
                   const QSharedPointer<SceneElement<float>>& scene) noexcept
        : parent(std::move(parent)), undoStack(undoStack), scene(scene)
    { }

    bool popUpdate();
    inline void update() { needsUpdate = true; }

    virtual Qt::CursorShape cursorShape() const { return Qt::ArrowCursor; }

    virtual std::unique_ptr<MouseOperation> activate(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition);
    virtual std::unique_ptr<MouseOperation> cancelOperation(std::unique_ptr<MouseOperation>&& current);
    virtual std::unique_ptr<MouseOperation> mousePressed(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition);
    virtual std::unique_ptr<MouseOperation> mouseMoved(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition, Qt::MouseButtons buttons);
    virtual std::unique_ptr<MouseOperation> mouseReleased(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition, Qt::MouseButtons buttons);
    virtual std::unique_ptr<MouseOperation> mouseDoubleClicked(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition, Qt::MouseButtons buttons);

    virtual ~MouseOperation() { }

protected:
    std::unique_ptr<MouseOperation> parent;
    QSharedPointer<UndoStack> undoStack;
    QSharedPointer<SceneElement<float>> scene;

private:
    bool needsUpdate = false;
};

#endif // MOUSEOPERATION_H

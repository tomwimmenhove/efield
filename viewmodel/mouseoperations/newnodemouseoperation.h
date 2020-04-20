#ifndef NEWNODEMOUSEOPERATION_H
#define NEWNODEMOUSEOPERATION_H

#include "mouseoperation.h"

class NewNodeMouseOperation : public MouseOperation
{
public:
    NewNodeMouseOperation(std::unique_ptr<MouseOperation>&& parent,
                          const QSharedPointer<UndoStack>& undoStack,
                          const QSharedPointer<SceneElement<float>>& scene) noexcept
        : MouseOperation(std::move(parent), undoStack, scene)
    { }

    Qt::CursorShape cursorShape() const override { return Qt::CrossCursor; }

    void activate(std::unique_ptr<MouseOperation>& current, const QPoint& pointerPosition) override;
    void mousePressed(std::unique_ptr<MouseOperation>& current, const QPoint& pointerPosition) override;
    void cancelOperation(std::unique_ptr<MouseOperation>& current) override;
    void mouseMoved(std::unique_ptr<MouseOperation>& current, const QPoint& pointerPosition, Qt::MouseButtons buttons) override;

private:
    void placeNewNodeElement(const QPoint& pointerPosition);

    QPoint nodeSavedPos;
};

#endif // NEWNODEMOUSEOPERATION_H

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

    std::unique_ptr<MouseOperation> activate(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition) override;
    std::unique_ptr<MouseOperation> mousePressed(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition) override;
    std::unique_ptr<MouseOperation> cancelOperation(std::unique_ptr<MouseOperation>&& current) override;
    std::unique_ptr<MouseOperation> mouseMoved(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition, Qt::MouseButtons buttons) override;

private:
    void placeNewNodeElement(const QPoint& pointerPosition);

    QPoint nodeSavedPos;
};

#endif // NEWNODEMOUSEOPERATION_H

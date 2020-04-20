#ifndef NEWLINEMOUSEOPERATION_H
#define NEWLINEMOUSEOPERATION_H

#include "mouseoperation.h"

class NewLineMouseOperation : public MouseOperation
{
public:
    NewLineMouseOperation(std::unique_ptr<MouseOperation>&& parent,
                          const QSharedPointer<UndoStack>& undoStack,
                          const QSharedPointer<SceneElement<float>>& scene) noexcept
        : MouseOperation(std::move(parent), undoStack, scene)
    { }

    Qt::CursorShape cursorShape() const override { return Qt::CrossCursor; }

    std::unique_ptr<MouseOperation> mousePressed(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition) override;
    std::unique_ptr<MouseOperation> cancelOperation(std::unique_ptr<MouseOperation>&& current) override;
    std::unique_ptr<MouseOperation> mouseMoved(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition, Qt::MouseButtons buttons) override;

private:
    QPoint nodeSavedPos;

    enum class State
    {
        p1,
        p2
    };

    int startId;

    State state = State::p1;
};

#endif // NEWLINEMOUSEOPERATION_H

#ifndef SELECTIONMOUSEOPERATION_H
#define SELECTIONMOUSEOPERATION_H

#include "mouseoperation.h"

class SelectionMouseOperation : public MouseOperation
{
public:
    SelectionMouseOperation(std::unique_ptr<MouseOperation>&& parent,
                       const QSharedPointer<UndoStack>& undoStack,
                       const QSharedPointer<SceneElement<float>>& scene) noexcept
        : MouseOperation(std::move(parent), undoStack, scene)
    { }

    std::unique_ptr<MouseOperation> activate(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition) override;
    std::unique_ptr<MouseOperation> cancelOperation(std::unique_ptr<MouseOperation>&& current) override;
    std::unique_ptr<MouseOperation> mouseMoved(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition, Qt::MouseButtons buttons) override;
    std::unique_ptr<MouseOperation> mouseReleased(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition, Qt::MouseButtons buttons) override;

private:
    QPoint dragStartPos;
};
#endif // SELECTIONMOUSEOPERATION_H

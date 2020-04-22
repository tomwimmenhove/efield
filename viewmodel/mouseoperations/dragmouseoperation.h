#ifndef DRAGNODEMOUSEOPERATION_H
#define DRAGNODEMOUSEOPERATION_H

#include <QMap>

#include "mouseoperation.h"
#include "util/undo/compositundonamegenerator.h"

class DragMouseOperation : public MouseOperation
{
public:
    DragMouseOperation(std::unique_ptr<MouseOperation>&& parent,
                       const QSharedPointer<UndoStack>& undoStack,
                       const QSharedPointer<SceneElement<float>>& scene) noexcept
        : MouseOperation(std::move(parent), undoStack, scene)
    { }

    Qt::CursorShape cursorShape() const override { return Qt::CrossCursor; }

    std::unique_ptr<MouseOperation> activate(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition) override;
    std::unique_ptr<MouseOperation> cancelOperation(std::unique_ptr<MouseOperation>&& current) override;
    std::unique_ptr<MouseOperation> mouseMoved(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition, Qt::MouseButtons buttons) override;
    std::unique_ptr<MouseOperation> mouseReleased(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition, Qt::MouseButtons buttons) override;

private:
    QMap<int, QPoint> savedPositions;
    bool started = false;
    QPoint dragStartPos;
    QRect dragStartSelectionBounds;
    CompositUndoNameGenerator nameGen = CompositUndoNameGenerator("Move");
};

#endif // DRAGNODEMOUSEOPERATION_H

#ifndef DRAGNODEMOUSEOPERATION_H
#define DRAGNODEMOUSEOPERATION_H

#include <QMap>

#include "mouseoperation.h"

class DragMouseOperation : public MouseOperation
{
public:
    DragMouseOperation(std::unique_ptr<MouseOperation>&& parent,
                       const QSharedPointer<UndoStack>& undoStack,
                       const QSharedPointer<SceneElement<float>>& scene) noexcept
        : MouseOperation(std::move(parent), undoStack, scene)
    { }

    Qt::CursorShape cursorShape() const override { return Qt::CrossCursor; }

    void activate(std::unique_ptr<MouseOperation>& current, const QPoint& pointerPosition) override;
    void cancelOperation(std::unique_ptr<MouseOperation>& current) override;
    void mouseMoved(std::unique_ptr<MouseOperation>& current, const QPoint& pointerPosition) override;
    void mouseReleased(std::unique_ptr<MouseOperation>& current, const QPoint& pointerPosition, Qt::MouseButtons buttons) override;

private:
    QMap<int, QPoint> savedPositions;
    bool started = false;
    QPoint dragStartPos;
    QRect dragStartSelectionBounds;
};

#endif // DRAGNODEMOUSEOPERATION_H

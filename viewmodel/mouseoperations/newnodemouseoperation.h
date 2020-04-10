#ifndef NEWNODEMOUSEOPERATION_H
#define NEWNODEMOUSEOPERATION_H

#include "mouseoperation.h"

class NewNodeMouseOperation : public MouseOperation
{
public:
    NewNodeMouseOperation(std::unique_ptr<MouseOperation>&& parent, QSharedPointer<SceneElement<float>> scene) noexcept
        : MouseOperation(std::move(parent), scene)
    { }

    Qt::CursorShape cursorShape() const override { return Qt::CrossCursor; }

    void mousePressed(std::unique_ptr<MouseOperation>& current, const QPoint& pointerPosition) override;
    void cancelOperation(std::unique_ptr<MouseOperation>& current) override;
    void mouseMoved(std::unique_ptr<MouseOperation>& current, const QPoint& pointerPosition) override;

private:
    void placeNewNodeElement(const QPoint& pointerPosition);

    QPoint nodeSavedPos;
};

#endif // NEWNODEMOUSEOPERATION_H

#ifndef DRAGNODEMOUSEOPERATION_H
#define DRAGNODEMOUSEOPERATION_H

#include "mouseoperation.h"

class DragNodeMouseOperation : public MouseOperation
{
public:
    DragNodeMouseOperation(std::unique_ptr<MouseOperation>&& parent, QSharedPointer<SceneElement<float>> scene) noexcept
        : MouseOperation(std::move(parent), scene)
    { }

    Qt::CursorShape cursorShape() const { return Qt::CrossCursor; }

    void activateOperation(std::unique_ptr<MouseOperation>& current, const QPoint&)
    {
        scene->highlight(scene->end());
        current = std::move(parent);
    }

    void cancelOperation(std::unique_ptr<MouseOperation>& current)
    {
        auto highLighted = scene->findHighLighted();
        Q_ASSERT(highLighted->elementType() == drawingElementType::Node);
        NodeElement<float>& node = static_cast<NodeElement<float>&>(*highLighted);
        node.node().setPoint(nodeSavedPos);

        current = std::move(parent);
    }

    void mouseMoved(std::unique_ptr<MouseOperation>&, const QPoint& pointerPosition)
    {
        auto highLighted = scene->findHighLighted();
        Q_ASSERT(highLighted != scene->end());
        Q_ASSERT(highLighted->elementType() == drawingElementType::Node);
        NodeElement<float>& node = static_cast<NodeElement<float>&>(*highLighted);
        node.node().setPoint(pointerPosition);
    }

    void mouseReleased(std::unique_ptr<MouseOperation>& current, const QPoint&, Qt::MouseButtons buttons)
    {
        if (buttons == Qt::RightButton)
            return;

        current = std::move(parent);
    }

private:
    QPoint nodeSavedPos;
};
#endif // DRAGNODEMOUSEOPERATION_H

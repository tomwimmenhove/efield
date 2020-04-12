#include "dragnodemouseoperation.h"

void DragNodeMouseOperation::activate(std::unique_ptr<MouseOperation>&, const QPoint& pointerPosition)
{
    auto closest = scene->closestElement(pointerPosition);
    Q_ASSERT(closest != scene->end());
    Q_ASSERT(closest->elementType() == drawingElementType::Node);
    auto& closestNode = static_cast<NodeElement<float>&>(*closest);
    nodeSavedPos = closestNode.node().point();
    scene->highlight(closest);

    update();
}

void DragNodeMouseOperation::cancelOperation(std::unique_ptr<MouseOperation>& current)
{
    auto highLighted = scene->findHighLighted();
    Q_ASSERT(highLighted != scene->end());
    Q_ASSERT(highLighted->elementType() == drawingElementType::Node);
    auto& node = static_cast<NodeElement<float>&>(*highLighted);
    node.node().setPoint(nodeSavedPos);

    parent->update();
    current = std::move(parent);
}

void DragNodeMouseOperation::mouseMoved(std::unique_ptr<MouseOperation>&, const QPoint& pointerPosition)
{
    auto highLighted = scene->findHighLighted();
    Q_ASSERT(highLighted != scene->end());
    Q_ASSERT(highLighted->elementType() == drawingElementType::Node);
    auto& node = static_cast<NodeElement<float>&>(*highLighted);
    node.node().setPoint(pointerPosition);

    update();
}

void DragNodeMouseOperation::mouseReleased(std::unique_ptr<MouseOperation>& current, const QPoint&, Qt::MouseButtons buttons)
{
    if (buttons != Qt::LeftButton)
        current = std::move(parent);
}

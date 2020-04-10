#include "dragnodemouseoperation.h"

void DragNodeMouseOperation::activated(std::unique_ptr<MouseOperation>&, const QPoint& pointerPosition)
{
    auto closest = scene->closestElement(pointerPosition);
    Q_ASSERT(closest->elementType() == drawingElementType::Node);
    scene->highlight(closest);
}

void DragNodeMouseOperation::cancelOperation(std::unique_ptr<MouseOperation>& current)
{
    auto highLighted = scene->findHighLighted();
    Q_ASSERT(highLighted->elementType() == drawingElementType::Node);
    NodeElement<float>& node = static_cast<NodeElement<float>&>(*highLighted);
    node.node().setPoint(nodeSavedPos);

    current = std::move(parent);
}

void DragNodeMouseOperation::mouseMoved(std::unique_ptr<MouseOperation>&, const QPoint& pointerPosition)
{
    auto highLighted = scene->findHighLighted();
    Q_ASSERT(highLighted != scene->end());
    Q_ASSERT(highLighted->elementType() == drawingElementType::Node);
    NodeElement<float>& node = static_cast<NodeElement<float>&>(*highLighted);
    node.node().setPoint(pointerPosition);
}

void DragNodeMouseOperation::mouseReleased(std::unique_ptr<MouseOperation>& current, const QPoint&, Qt::MouseButtons buttons)
{
    if (buttons == Qt::RightButton)
        return;

    current = std::move(parent);
}

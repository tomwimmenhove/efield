#include "dragnodemouseoperation.h"

void DragNodeMouseOperation::activate(std::unique_ptr<MouseOperation>&, const QPoint& pointerPosition)
{
    auto closest = scene->closestElement(pointerPosition);
    Q_ASSERT(closest->elementType() == drawingElementType::Node);
    scene->highlight(closest);

    update = true;
}

void DragNodeMouseOperation::cancelOperation(std::unique_ptr<MouseOperation>& current)
{
    auto highLighted = scene->findHighLighted();
    Q_ASSERT(highLighted->elementType() == drawingElementType::Node);
    NodeElement<float>& node = static_cast<NodeElement<float>&>(*highLighted);
    node.node().setPoint(nodeSavedPos);

    update = true;
    current = std::move(parent);
}

void DragNodeMouseOperation::mouseMoved(std::unique_ptr<MouseOperation>&, const QPoint& pointerPosition)
{
    auto highLighted = scene->findHighLighted();
    Q_ASSERT(highLighted != scene->end());
    Q_ASSERT(highLighted->elementType() == drawingElementType::Node);
    NodeElement<float>& node = static_cast<NodeElement<float>&>(*highLighted);
    node.node().setPoint(pointerPosition);

    update = true;
}

void DragNodeMouseOperation::mouseReleased(std::unique_ptr<MouseOperation>& current, const QPoint&, Qt::MouseButtons buttons)
{
    if (buttons == Qt::RightButton)
        return;

    current = std::move(parent);
}

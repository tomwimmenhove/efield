#include "normalmouseoperation.h"
#include "dragnodemouseoperation.h"

void NormalMouseOperation::mousePressed(std::unique_ptr<MouseOperation>& current, const QPoint& pointerPosition)
{
    auto closest = scene->closestElement(pointerPosition);
    if (closest != scene->end() && closest->elementType() == drawingElementType::Node)
    {
        current = std::make_unique<DragNodeMouseOperation>(std::move(current), scene);
        current->activated(current, pointerPosition);
        return;
    }

    scene->highlight(closest);
}

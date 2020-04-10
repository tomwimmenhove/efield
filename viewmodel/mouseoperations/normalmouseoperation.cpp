#include <QInputDialog>

#include "graphics/nodeelement.h"
#include "graphics/lineelement.h"
#include "normalmouseoperation.h"
#include "dragnodemouseoperation.h"
#include "pointinputdialog.h"

void NormalMouseOperation::mousePressed(std::unique_ptr<MouseOperation>& current, const QPoint& pointerPosition)
{
    auto closest = scene->closestElement(pointerPosition);
    if (closest != scene->end() && closest->elementType() == drawingElementType::Node)
    {
        current = std::make_unique<DragNodeMouseOperation>(std::move(current), scene);
        current->activate(current, pointerPosition);
//        return;
    }

    scene->highlight(closest);

    update = true;
}

void NormalMouseOperation::mouseDoubleClicked(std::unique_ptr<MouseOperation>&, const QPoint& pointerPosition, Qt::MouseButtons buttons)
{
    if (buttons != Qt::LeftButton)
        return;

    auto closest = scene->closestElement(pointerPosition);
    if (closest == scene->end())
        return;

    switch (closest->elementType())
    {
        case drawingElementType::None:
        case drawingElementType::Scene:
            break;
        case drawingElementType::Node:
            emit editNode(static_cast<NodeElement<float>&>(*closest));
            break;
        case drawingElementType::Line:
            emit editLine(static_cast<LineElement<float>&>(*closest));
            break;
    }
}

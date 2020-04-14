#include <QInputDialog>

#include "graphics/nodeelement.h"
#include "graphics/lineelement.h"
#include "normalmouseoperation.h"
#include "dragmouseoperation.h"
#include "pointinputdialog.h"

void NormalMouseOperation::mousePressed(std::unique_ptr<MouseOperation>& current, const QPoint& pointerPosition)
{
    auto closest = scene->closestElement(pointerPosition);
    if (closest != scene->end())
    {
        current = std::make_unique<DragMouseOperation>(std::move(current), undoStack, scene);
        current->activate(current, pointerPosition);
        return;
    }

    scene->highlight(closest);

    update();
}

void NormalMouseOperation::mouseDoubleClicked(std::unique_ptr<MouseOperation>&, const QPoint& pointerPosition, Qt::MouseButtons buttons)
{
    if (buttons != Qt::LeftButton)
        return;

    auto closest = scene->closestElement(pointerPosition);
    if (closest != scene->end())
        emit editElement(*closest);
}

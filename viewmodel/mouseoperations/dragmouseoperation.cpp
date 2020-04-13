#include "dragmouseoperation.h"

void DragMouseOperation::activate(std::unique_ptr<MouseOperation>&, const QPoint& pointerPosition)
{
    auto closest = scene->closestElement(pointerPosition);
    Q_ASSERT(closest != scene->end());
    savedPos = closest->center();
    dragStartPos = pointerPosition;
    scene->highlight(closest);

    update();
}

void DragMouseOperation::cancelOperation(std::unique_ptr<MouseOperation>& current)
{
    auto highLighted = scene->findHighLighted();
    Q_ASSERT(highLighted != scene->end());
    highLighted->setCenter(savedPos);

    parent->update();
    current = std::move(parent);
}

void DragMouseOperation::mouseMoved(std::unique_ptr<MouseOperation>&, const QPoint& pointerPosition)
{
    auto highLighted = scene->findHighLighted();
    Q_ASSERT(highLighted != scene->end());
    highLighted->setCenter(savedPos + pointerPosition - dragStartPos);

    update();
}

void DragMouseOperation::mouseReleased(std::unique_ptr<MouseOperation>& current, const QPoint&, Qt::MouseButtons buttons)
{
    if (buttons != Qt::LeftButton)
        current = std::move(parent);
}
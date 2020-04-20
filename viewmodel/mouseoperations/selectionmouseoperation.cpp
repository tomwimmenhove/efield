#include "selectionmouseoperation.h"

std::unique_ptr<MouseOperation> SelectionMouseOperation::activate(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition)
{
    scene->highlightExclusive(scene->end());

    dragStartPos = pointerPosition;
    scene->setSelectionRect(QRect());

    return std::move(current);
}

std::unique_ptr<MouseOperation> SelectionMouseOperation::cancelOperation(std::unique_ptr<MouseOperation>&&)
{
    scene->setSelectionRect(QRect());

    parent->update();
    return std::move(parent);
}

std::unique_ptr<MouseOperation> SelectionMouseOperation::mouseMoved(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition, Qt::MouseButtons)
{
    QRect rect(dragStartPos, pointerPosition);
    scene->setSelectionRect(rect);

    for (auto i = scene->begin(); i != scene->end(); ++i)
        i->setHighlighted(rect.contains(i->center()));

    update();

    return std::move(current);
}

std::unique_ptr<MouseOperation> SelectionMouseOperation::mouseReleased(std::unique_ptr<MouseOperation>&& current, const QPoint&, Qt::MouseButtons buttons)
{
    if (buttons != Qt::LeftButton)
    {
        scene->setSelectionRect(QRect());

        parent->update();
        return std::move(parent);
    }

    return std::move(current);
}

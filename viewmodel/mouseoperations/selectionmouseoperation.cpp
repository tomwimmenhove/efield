#include "selectionmouseoperation.h"

void SelectionMouseOperation::activate(std::unique_ptr<MouseOperation>&, const QPoint& pointerPosition)
{
    scene->highlightUnique(scene->end());

    dragStartPos = pointerPosition;
    scene->setSelectionRect(QRect());
}

void SelectionMouseOperation::cancelOperation(std::unique_ptr<MouseOperation>& current)
{
    current = std::move(parent);
    scene->setSelectionRect(QRect());
}

void SelectionMouseOperation::mouseMoved(std::unique_ptr<MouseOperation>&, const QPoint& pointerPosition)
{
    QRect rect(dragStartPos, pointerPosition);
    scene->setSelectionRect(rect);

    for (auto i = scene->begin(); i != scene->end(); ++i)
        i->setHighlighted(rect.contains(i->center()));

    update();
}

void SelectionMouseOperation::mouseReleased(std::unique_ptr<MouseOperation>& current, const QPoint&, Qt::MouseButtons buttons)
{
    if (buttons != Qt::LeftButton)
    {
        scene->setSelectionRect(QRect());
        current = std::move(parent);
    }
}

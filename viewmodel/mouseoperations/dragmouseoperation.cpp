#include "dragmouseoperation.h"
#include "util/undo/moveundoitem.h"

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
    if (!started)
    {
        QPoint p = pointerPosition - dragStartPos;
        if (p.manhattanLength() > 5)
            started = true;
    }

    if (!started)
        return;

    auto highLighted = scene->findHighLighted();
    Q_ASSERT(highLighted != scene->end());
    highLighted->setCenter(savedPos + pointerPosition - dragStartPos);

    update();
}

void DragMouseOperation::mouseReleased(std::unique_ptr<MouseOperation>& current, const QPoint&, Qt::MouseButtons buttons)
{
    if (buttons != Qt::LeftButton)
        current = std::move(parent);

    auto highLighted = scene->findHighLighted();
    Q_ASSERT(highLighted != scene->end());

    if (savedPos != highLighted->center())
        undoStack->add(std::make_unique<MoveUndoItem>(scene, highLighted->identifier(), savedPos, highLighted->center(), "Move"));
}

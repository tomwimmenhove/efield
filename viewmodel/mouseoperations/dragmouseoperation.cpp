#include "dragmouseoperation.h"
#include "util/undo/moveundoitem.h"
#include "util/undo/compositundoitem.h"

void DragMouseOperation::activate(std::unique_ptr<MouseOperation>&, const QPoint& pointerPosition)
{
    auto closest = scene->closestElement(pointerPosition);
    Q_ASSERT(closest != scene->end());
    dragStartPos = pointerPosition;

    /* If it's not a multi-selection, highlight the one we clicked */
    if (scene->findFirstHighLighted() == scene->end())
        scene->highlightUnique(closest);

    /* Exclude elements that are marked as 'in use'. If we're moving any elements that are
     * using those elements, they will be moved by those elements, indirectly */
    for(auto i = scene->begin(); i != scene->end(); ++i)
        if (i->isHighlighted() && !i->isInUse())
            savedPositions[i->identifier()] = i->center();

    update();
}

void DragMouseOperation::cancelOperation(std::unique_ptr<MouseOperation>& current)
{
    QMapIterator<int, QPoint> i(savedPositions);
    while (i.hasNext())
    {
        i.next();
        auto it = scene->findId(i.key());
        Q_ASSERT(it != scene->end());
        it->setCenter(i.value());
    }

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

    QMapIterator<int, QPoint> i(savedPositions);
    while (i.hasNext())
    {
        i.next();
        auto it = scene->findId(i.key());
        Q_ASSERT(it != scene->end());
        it->setCenter(i.value() + pointerPosition - dragStartPos);
    }

    update();
}

void DragMouseOperation::mouseReleased(std::unique_ptr<MouseOperation>& current, const QPoint&, Qt::MouseButtons buttons)
{
    if (buttons & Qt::LeftButton)
        return;

    if (started && !savedPositions.empty())
    {
        auto composit = std::make_unique<CompositUndoItem>(scene, "Move");

        QMapIterator<int, QPoint> i(savedPositions);
        while (i.hasNext())
        {
            i.next();
            auto it = scene->findId(i.key());
            Q_ASSERT(it != scene->end());
            composit->add(std::make_unique<MoveUndoItem>(
                              scene, it->identifier(), i.value(), it->center(), "Move"));
        }

        undoStack->add(std::move(composit));
    }

    current = std::move(parent);
}

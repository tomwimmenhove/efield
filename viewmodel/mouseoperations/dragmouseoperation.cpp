#include "dragmouseoperation.h"
#include "util/undo/moveundoitem.h"
#include "util/undo/compositundoitem.h"
#include "util/undo/compositundonamegenerator.h"

void DragMouseOperation::activate(std::unique_ptr<MouseOperation>&, const QPoint& pointerPosition)
{
    auto closest = scene->closestElement(pointerPosition);
    Q_ASSERT(closest != scene->end());
    dragStartPos = pointerPosition;

    int numHighlighted;

    if (!closest->isHighlighted())
    {
        scene->highlightExclusive(closest);
        numHighlighted = 1;
    }
    else
        numHighlighted = scene->numHighlighted();

    /* Exclude elements that are marked as 'in use'. If we're moving any elements that are
     * using those elements, they will be moved by those elements, indirectly */
    for(auto i = scene->begin(); i != scene->end(); ++i)
        if (i->isHighlighted() && (numHighlighted == 1 || !i->isInUse()))
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
        QSharedPointer<UndoStack> nestedUndoStack = QSharedPointer<UndoStack>::create();

        CompositUndoNameGenerator nameGen("Move");

        QMapIterator<int, QPoint> i(savedPositions);
        while (i.hasNext())
        {
            i.next();
            auto it = scene->findId(i.key());
            Q_ASSERT(it != scene->end());

            nameGen.addElementName(it->name());

            auto undoItem = std::make_unique<MoveUndoItem>(scene, it->identifier(), i.value(), it->center());
            undoItem->doFunction();
            nestedUndoStack->add(std::move(undoItem));
        }

        undoStack->add(std::make_unique<CompositUndoItem>(scene, nestedUndoStack, nameGen.generate()));
    }

    current = std::move(parent);
}

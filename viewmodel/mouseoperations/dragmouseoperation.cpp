#include <QGuiApplication>

#include "dragmouseoperation.h"
#include "util/undo/moveundoitem.h"
#include "util/undo/compositundoitem.h"
#include "util/undo/compositundonamegenerator.h"

std::unique_ptr<MouseOperation> DragMouseOperation::activate(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition)
{
    auto closest = scene->closestElement(pointerPosition);
    Q_ASSERT(closest != scene->end());

    /* Exclude elements that are marked as 'in use'. If we're moving any elements that are
     * using those elements, they will be moved by those elements, indirectly */
    for(auto i = scene->begin(); i != scene->end(); ++i)
        if (i->isHighlighted() && (scene->numHighlighted() == 1 || !i->isInUse()))
            savedPositions[i->identifier()] = i->center();

    dragStartPos = pointerPosition;
    dragStartSelectionBounds = scene->selectionBounds();

    update();

    return std::move(current);
}

std::unique_ptr<MouseOperation> DragMouseOperation::cancelOperation(std::unique_ptr<MouseOperation>&&)
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

    return std::move(parent);
}

std::unique_ptr<MouseOperation> DragMouseOperation::mouseMoved(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition, Qt::MouseButtons)
{
    if (!started)
    {
        QPoint p = pointerPosition - dragStartPos;
        if (p.manhattanLength() > 5)
            started = true;
    }

    if (!started)
        return std::move(current);

    QRect rect = dragStartSelectionBounds.translated(pointerPosition - dragStartPos);
    QRect clipped = Geometry::clip(rect, scene->sceneBounds());
    auto delta = clipped.topLeft() - dragStartSelectionBounds.topLeft();

    QMapIterator<int, QPoint> i(savedPositions);
    while (i.hasNext())
    {
        i.next();
        auto it = scene->findId(i.key());
        Q_ASSERT(it != scene->end());
        it->setCenter(i.value() + delta);
    }

    update();

    return std::move(current);
}

std::unique_ptr<MouseOperation> DragMouseOperation::mouseReleased(std::unique_ptr<MouseOperation>&& current, const QPoint&, Qt::MouseButtons buttons)
{
    if (buttons & Qt::LeftButton)
        return std::move(current);;

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

    return std::move(parent);
}

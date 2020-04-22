#include <QGuiApplication>

#include "dragmouseoperation.h"
#include "util/undo/moveundoitem.h"
#include "util/undo/compositundoitem.h"

std::unique_ptr<MouseOperation> DragMouseOperation::activate(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition)
{
    auto closest = scene->closestElement(pointerPosition);
    Q_ASSERT(closest != scene->end());

    for(auto i = scene->begin(); i != scene->end(); ++i)
    {
        if (i->isHighlighted())
        {
            nameGen.addElementName(i->name());

            if (i->canAnchor())
                savedPositions[i->identifier()] = i->center();
            else
            {
                for(auto id: i->uses())
                {
                    auto it = scene->findId(id);
                    Q_ASSERT(it != scene->end());
                    savedPositions[id] = it->center();
                }
            }
        }
    }

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

        QMapIterator<int, QPoint> i(savedPositions);
        while (i.hasNext())
        {
            i.next();
            auto it = scene->findId(i.key());
            Q_ASSERT(it != scene->end());

            auto undoItem = std::make_unique<MoveUndoItem>(scene, it->identifier(), i.value(), it->center());
            undoItem->doFunction();
            nestedUndoStack->add(std::move(undoItem));
        }

        undoStack->add(std::make_unique<CompositUndoItem>(scene, nestedUndoStack, nameGen.generate()));
    }

    return std::move(parent);
}

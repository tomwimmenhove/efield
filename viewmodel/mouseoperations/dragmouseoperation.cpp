#include <QGuiApplication>

#include "viewmodel/elementdependencyvisitor.h"
#include "dragmouseoperation.h"
#include "util/undo/moveundoitem.h"
#include "util/undo/compositundoitem.h"

std::unique_ptr<MouseOperation> DragMouseOperation::activate(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition)
{
    for(auto& element: *scene)
        if (element.isHighlighted())
            nameGen.addElementName(element.name());

    ElementDependencyVisitor deps;
    deps.allHighlighted(*scene);

    for(auto id: deps.dependencies())
    {
        auto it = scene->findId(id);
        Q_ASSERT(it != scene->end());
        if (it->canAnchor())
            savedPositions[id] = it->center();
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
    QRect rect = dragStartSelectionBounds.translated(pointerPosition - dragStartPos);
    QRect clipped = Geometry::clip(rect, scene->sceneBounds());
    QPoint delta = clipped.topLeft() - dragStartSelectionBounds.topLeft();

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

    if (!savedPositions.empty())
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

        undoStack->add(std::make_unique<CompositUndoItem>(nestedUndoStack, nameGen.generate()));
    }

    return std::move(parent);
}

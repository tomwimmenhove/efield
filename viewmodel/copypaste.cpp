#include "copypaste.h"
#include "copydrawingelementvisitor.h"
#include "util/undo/compositundoitem.h"
#include "util/undo/moveundoitem.h"
#include "deletedrawingelementvisitor.h"

void CopyPaste::copySelection(bool deleteAfter)
{
    clipBoardScene->clear();

    CopyDrawingElementVisitor copy(scene, clipBoardScene);

    for(auto& element: *scene)
        if (element.isHighlighted())
            element.accept(copy);

    if (deleteAfter)
        DeleteDrawingElementVisitor::deleteSelected(undoStack, scene);

    scene->highlightExclusive(scene->end());
}

bool CopyPaste::pasteFits()
{
    QRect clipBoardBounds = clipBoardScene->selectionBounds();
    return clipBoardBounds.width() <= scene->sceneSize().width() &&
           clipBoardBounds.height() <= scene->sceneSize().height();
}

void CopyPaste::paste()
{
    QSharedPointer<UndoStack> nestedUndoStack = QSharedPointer<UndoStack>::create();

    scene->highlightExclusive(scene->end());

    /* Paste all elements */
    CopyDrawingElementVisitor paste(clipBoardScene, scene, nestedUndoStack);
    for(auto& element: *clipBoardScene)
            element.accept(paste);

    /* And center them */
    QRect rect = scene->selectionBounds().translated(scene->center()- scene->selectionBounds().center());
    QRect clipped = Geometry::clip(rect, scene->sceneSize());
    QPoint delta = clipped.topLeft() - scene->selectionBounds().topLeft();
    for(auto& element: *scene)
    {
        if (element.isHighlighted() && element.canAnchor())
        {
            auto undoItem = std::make_unique<MoveUndoItem>
                            (scene, element.identifier(), element.center(), element.center() + delta);
            undoItem->doFunction();
            nestedUndoStack->add(std::move(undoItem));
        }
    }

    undoStack->add(std::make_unique<CompositUndoItem>( nestedUndoStack, "Paste"));
}

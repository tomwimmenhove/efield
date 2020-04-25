#include "copypaste.h"
#include "copydrawingelementvisitor.h"
#include "util/undo/compositundoitem.h"
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
    return clipBoardBounds.width() <= scene->sceneBounds().width() &&
           clipBoardBounds.height() <= scene->sceneBounds().height();
}

void CopyPaste::paste()
{
    QSharedPointer<UndoStack> nestedUndoStack = QSharedPointer<UndoStack>::create();

    scene->highlightExclusive(scene->end());

    CopyDrawingElementVisitor paste(clipBoardScene, scene, nestedUndoStack);
    for(auto& element: *clipBoardScene)
            element.accept(paste);

    undoStack->add(std::make_unique<CompositUndoItem>(scene, nestedUndoStack, "Paste"));

    QRect rect = scene->selectionBounds().translated(scene->center()- scene->selectionBounds().center());
    QRect clipped = Geometry::clip(rect, scene->sceneBounds());
    QPoint delta = clipped.topLeft() - scene->selectionBounds().topLeft();
    for(auto& element: *scene)
        if (element.isHighlighted() && element.canAnchor())
            element.setCenter(element.center() + delta);
}

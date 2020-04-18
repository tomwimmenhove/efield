#include "compositundoitem.h"

CompositUndoItem::CompositUndoItem(const QSharedPointer<SceneElement<float>>& scene,
                                   const QSharedPointer<UndoStack>& undoStack,
                                   const QString& title)
    : UndoItem(scene), undoStack(undoStack), titleString(title)
{ }

void CompositUndoItem::undoFunction()
{
    undoStack->undoAll();
}

void CompositUndoItem::doFunction()
{
    undoStack->redoAll();
}

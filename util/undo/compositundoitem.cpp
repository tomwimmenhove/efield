#include "compositundoitem.h"

CompositUndoItem::CompositUndoItem(const QSharedPointer<SceneElement<float>>& scene, const QSharedPointer<UndoStack>& undoStack, const QString& title)
    : UndoItem(scene, title), undoStack(undoStack)
{ }

void CompositUndoItem::undoFunction()
{
    undoStack->undoAll();
}

void CompositUndoItem::doFunction()
{
    undoStack->redoAll();
}

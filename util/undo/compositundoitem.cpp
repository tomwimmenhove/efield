#include "compositundoitem.h"

CompositUndoItem::CompositUndoItem(const QSharedPointer<UndoStack>& undoStack,
                                   const QString& title)
    : undoStack(undoStack), titleString(title)
{ }

void CompositUndoItem::undoFunction()
{
    undoStack->undoAll();
}

void CompositUndoItem::doFunction()
{
    undoStack->redoAll();
}

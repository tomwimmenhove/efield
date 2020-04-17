#include "undostack.h"

void UndoStack::undo()
{
    Q_ASSERT(canUndo());
    stack[--pos]->undoFunction();
}

QString UndoStack::undoName() const
{
    Q_ASSERT(canUndo());
    return stack[pos - 1]->title();
}

void UndoStack::redo()
{
    Q_ASSERT(canRedo());
    stack[pos++]->doFunction();
}

QString UndoStack::redoName() const
{
    Q_ASSERT(canRedo());
    return stack[pos]->title();
}

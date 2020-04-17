#include "undostack.h"

void UndoStack::add(std::unique_ptr<UndoItem>&& item)
{
    Q_ASSERT(pos <= stack.size());
    if (pos < stack.size())
        stack.erase(stack.begin() + pos, stack.end());
    stack.push_back(std::move(item));

    ++pos;

    emitUpdate();
}

void UndoStack::undo()
{
    Q_ASSERT(canUndo());
    stack[--pos]->undoFunction();

    emitUpdate();
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

    emitUpdate();
}

QString UndoStack::redoName() const
{
    Q_ASSERT(canRedo());
    return stack[pos]->title();
}

void UndoStack::emitUpdate()
{
    bool cu = canUndo();
    bool cr = canRedo();

    emit stackUpdated(cu, cu ? undoName() : "", cr, cr ? redoName() : "");
}

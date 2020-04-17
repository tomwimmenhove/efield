#ifndef UNDOSTACK_H
#define UNDOSTACK_H

#include <QtGlobal>
#include <QString>
#include <vector>
#include <memory>

#include "undoitem.h"

class UndoStack
{
public:
    void add(std::unique_ptr<UndoItem>&& item)
    {
        Q_ASSERT(pos <= stack.size());
        if (pos < stack.size())
            stack.erase(stack.begin() + pos, stack.end());
        stack.push_back(std::move(item));

        ++pos;
    }

    inline bool canUndo() const { return pos > 0; }
    void undo();
    QString undoName() const;

    inline bool canRedo() const { return pos < stack.size(); }
    void redo();
    QString redoName() const;

private:
    size_t pos = 0;
    std::vector<std::unique_ptr<UndoItem>> stack;
};

#endif // UNDOSTACK_H

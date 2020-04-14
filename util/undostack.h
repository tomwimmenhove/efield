#ifndef UNDOSTACK_H
#define UNDOSTACK_H

#include <QtGlobal>
#include <QString>
#include <vector>
#include <functional>

class UndoItem
{
public:
    UndoItem(QString title, std::function<void()> undoFunc, std::function<void()> redoFunc)
        : t(title), undoFunc(undoFunc), redoFunc(redoFunc)
    { }

    inline QString title() const { return t; }
    inline void undo() { undoFunc(); }
    inline void redo() { redoFunc(); }

private:
    QString t;
    std::function<void()> undoFunc;
    std::function<void()> redoFunc;
};

class UndoStack
{
public:
    void add(const UndoItem& item)
    {
        Q_ASSERT(pos <= stack.size());
        if (pos < stack.size())
            stack.erase(stack.begin() + pos, stack.end());
        stack.push_back(item);

        ++pos;
    }

    inline bool canUndo() const { return pos > 0; }
    void undo()
    {
        Q_ASSERT(canUndo());
        stack[--pos].undo();
    }

    QString undoName() const
    {
        Q_ASSERT(canUndo());
        return stack[pos - 1].title();
    }

    inline bool canRedo() const { return pos < stack.size(); }
    void redo()
    {
        Q_ASSERT(canRedo());
        stack[pos++].redo();
    }

    QString redoName() const
    {
        Q_ASSERT(canRedo());
        return stack[pos].title();
    }

private:
    size_t pos = 0;
    std::vector<UndoItem> stack;
};

#endif // UNDOSTACK_H

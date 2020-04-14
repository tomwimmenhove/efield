#ifndef UNDOSTACK_H
#define UNDOSTACK_H

#include <QtGlobal>
#include <QString>
#include <vector>
#include <functional>

class UndoItem
{
public:
    UndoItem(const QString& title, std::function<void()> undoFunc, std::function<void()> doFunc)
        : titleString(title), undoFunc(undoFunc), doFunc(doFunc)
    { }

    inline QString title() const { return titleString; }
    inline void undoFunction() { undoFunc(); }
    inline void doFunction() { doFunc(); }

protected:
    UndoItem()
    { }

    UndoItem(const QString& title)
        : titleString(title)
    { }

    QString titleString;
    std::function<void()> undoFunc;
    std::function<void()> doFunc;
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
    void undo();
    QString undoName() const;

    inline bool canRedo() const { return pos < stack.size(); }
    void redo();
    QString redoName() const;

private:
    size_t pos = 0;
    std::vector<UndoItem> stack;
};

#endif // UNDOSTACK_H

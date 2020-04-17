#ifndef UNDOITEM_H
#define UNDOITEM_H

#include <QString>
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

#endif // UNDOITEM_H

#ifndef UNDOITEM_H
#define UNDOITEM_H

#include <QString>

class UndoItem
{
public:
    virtual QString title() const = 0;
    virtual void undoFunction() = 0;
    virtual void doFunction() = 0;

    virtual ~UndoItem() { }
};

#endif // UNDOITEM_H

#ifndef UNDOSTACK_H
#define UNDOSTACK_H

#include <QString>
#include <QObject>
#include <vector>
#include <memory>

#include "undoitem.h"

class UndoStack : public QObject
{
    Q_OBJECT

public:
    void add(std::unique_ptr<UndoItem>&& item);

    inline bool canUndo() const { return pos > 0; }
    void undo();
    QString undoName() const;
    void undoAll();

    inline bool canRedo() const { return pos < stack.size(); }
    void redo();
    QString redoName() const;
    void redoAll();

signals:
    void stackUpdated(bool canUndo, const QString& undoName, bool canRedo, const QString& redoName);

private:
    void emitUpdate();

    size_t pos = 0;
    std::vector<std::unique_ptr<UndoItem>> stack;
};

#endif // UNDOSTACK_H

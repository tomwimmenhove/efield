#ifndef COMPOSITUNDOITEM_H
#define COMPOSITUNDOITEM_H

#include <QSharedPointer>

#include "graphics/sceneelement.h"
#include "undostack.h"

class CompositUndoItem : public UndoItem
{
public:
    CompositUndoItem(const QSharedPointer<SceneElement<float>>& scene,
                     const QSharedPointer<UndoStack>& undoStack,
                     const QString& title);

    void undoFunction() override;
    void doFunction() override;

private:
    QSharedPointer<UndoStack> undoStack;
};

#endif // COMPOSITUNDOITEM_H

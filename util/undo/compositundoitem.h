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

    QString title() const override { return titleString; }

    void undoFunction() override;
    void doFunction() override;

private:
    QSharedPointer<UndoStack> undoStack;
    QString titleString;
};

#endif // COMPOSITUNDOITEM_H

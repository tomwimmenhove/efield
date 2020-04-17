#ifndef COMPOSITUNDOITEM_H
#define COMPOSITUNDOITEM_H

#include <QSharedPointer>

#include "graphics/sceneelement.h"
#include "undostack.h"

class CompositUndoItem : public UndoItem
{
public:
    CompositUndoItem(const QSharedPointer<SceneElement<float>>& scene,
                      const QString& title);

    void add(std::unique_ptr<UndoItem>&& item);

    void undoFunction() override;
    void doFunction() override;

private:
    std::vector<std::unique_ptr<UndoItem>> undoList;
};

#endif // COMPOSITUNDOITEM_H

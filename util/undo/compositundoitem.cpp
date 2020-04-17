#include "compositundoitem.h"

CompositUndoItem::CompositUndoItem(const QSharedPointer<SceneElement<float>>& scene, const QString& title)
    : UndoItem(scene, title)
{ }

void CompositUndoItem::add(std::unique_ptr<UndoItem>&& item)
{
    undoList.push_back(std::move(item));
}

void CompositUndoItem::undoFunction()
{
    for (auto i = undoList.rbegin(); i != undoList.rend(); ++i)
        (*i)->undoFunction();
}

void CompositUndoItem::doFunction()
{
    for(auto& e: undoList)
        e->doFunction();
}

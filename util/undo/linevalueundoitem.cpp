#include "linevalueundoitem.h"
#include "graphics/lineelement.h"

LineValueUndoItem::LineValueUndoItem(const QSharedPointer<SceneElement<float>>& scene,
                                     int id,
                                     float oldValue,
                                     float newValue)
    : UndoItem(scene), id(id), oldValue(oldValue), newValue(newValue)
{ }

void LineValueUndoItem::undoFunction()
{
    auto it = scene->findId(id);
    Q_ASSERT(it != scene->end());
    Q_ASSERT(typeid(*it).hash_code() == typeid(LineElement<float>).hash_code());
    static_cast<LineElement<float>&>(*it).setValue(oldValue);
}

void LineValueUndoItem::doFunction()
{
    auto it = scene->findId(id);
    Q_ASSERT(it != scene->end());
    Q_ASSERT(typeid(*it).hash_code() == typeid(LineElement<float>).hash_code());
    static_cast<LineElement<float>&>(*it).setValue(newValue);
}

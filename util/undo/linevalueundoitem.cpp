#include "linevalueundoitem.h"
#include "graphics/lineelement.h"

LineValueUndoItem::LineValueUndoItem(const QSharedPointer<SceneElement<float> >& scene,
                                     int id,
                                     float oldValue,
                                     float newValue,
                                     const QString& title)
    : UndoItem(title)
{
    undoFunc = [scene, id, oldValue]()
    {
        auto it = scene->findId(id);
        Q_ASSERT(it != scene->end());
        Q_ASSERT(it->elementType() == drawingElementType::Line);
        static_cast<LineElement<float>&>(*it).setValue(oldValue);
    };

    doFunc = [scene, id, newValue]()
    {
        auto it = scene->findId(id);
        Q_ASSERT(it != scene->end());
        Q_ASSERT(it->elementType() == drawingElementType::Line);
        static_cast<LineElement<float>&>(*it).setValue(newValue);
    };
}

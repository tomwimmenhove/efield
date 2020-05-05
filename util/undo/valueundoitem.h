#ifndef VALUEUNDOITEM_H
#define VALUEUNDOITEM_H

#include <QSharedPointer>

#include "graphics/sceneelement.h"
#include "undostack.h"

template<typename T>
class ValueUndoItem : public UndoItem
{
public:
    ValueUndoItem(const QSharedPointer<SceneElement<float>>& scene,
                  int id,
                  float oldValue,
                  float newValue)
        : scene(scene), id(id), oldValue(oldValue), newValue(newValue)
    { }

    void undoFunction() override
    {
        auto it = scene->findId(id);
        Q_ASSERT(it != scene->end());
        Q_ASSERT(typeid(*it).hash_code() == typeid(T).hash_code());
        static_cast<T&>(*it).setValue(oldValue);
    }
    void doFunction() override
    {
        auto it = scene->findId(id);
        Q_ASSERT(it != scene->end());
        Q_ASSERT(typeid(*it).hash_code() == typeid(T).hash_code());
        static_cast<T&>(*it).setValue(newValue);
    }

private:
    QSharedPointer<SceneElement<float>> scene;
    int id;
    float oldValue;
    float newValue;
};

#endif // VALUEUNDOITEM_H

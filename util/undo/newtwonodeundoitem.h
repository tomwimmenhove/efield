#ifndef NEWTWONODEUNDOITEM_H
#define NEWTWONODEUNDOITEM_H

#include <QSharedPointer>

#include "graphics/sceneelement.h"
#include "undostack.h"

template<typename T>
class NewTwoNodeUndoItem : public UndoItem
{
public:
    NewTwoNodeUndoItem(const QSharedPointer<SceneElement<float>>& scene,
                       int id, int nodeId1, int nodeId2, float value)
        : scene(scene), id(id), nodeId1(nodeId1), nodeId2(nodeId2), value(value)
    { }

    void undoFunction() override
    {
        auto it = scene->findId(id);
        Q_ASSERT(it != scene->end());
        Q_ASSERT(typeid(*it).hash_code() == typeid(T).hash_code());
        scene->remove(it);
    }

    void doFunction() override
    {
        auto it1 = scene->findId(nodeId1);
        Q_ASSERT(it1 != scene->end());
        Q_ASSERT(typeid(*it1).hash_code() == typeid(NodeElement<float>).hash_code());
        SharedNode node1 = static_cast<NodeElement<float>&>(*it1).anchorNode();

        auto it2 = scene->findId(nodeId2);
        Q_ASSERT(it2 != scene->end());
        Q_ASSERT(typeid(*it2).hash_code() == typeid(NodeElement<float>).hash_code());
        SharedNode node2 = static_cast<NodeElement<float>&>(*it2).anchorNode();

        auto element = std::make_unique<T>(id, scene->sceneSize(), node1, node2, value);
        scene->add(std::move(element));
    }

private:
    QSharedPointer<SceneElement<float>> scene;
    int id;
    int nodeId1;
    int nodeId2;
    float value;
};

#endif // NEWTWONODEUNDOITEM_H

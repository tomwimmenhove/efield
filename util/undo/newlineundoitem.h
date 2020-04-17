#ifndef PLACEUNDOITEM_H
#define PLACEUNDOITEM_H

#include <QSharedPointer>

#include "graphics/sceneelement.h"
#include "undostack.h"

class NewLineUndoItem : public UndoItem
{
public:
    NewLineUndoItem(const QSharedPointer<SceneElement<float>>& scene,
                  int id, int nodeId1, int nodeId2, float value,
                  const QString& title);

    void undoFunction() override;
    void doFunction() override;

private:
    int id;
    int nodeId1;
    int nodeId2;
    float value;
};

#endif // PLACEUNDOITEM_H

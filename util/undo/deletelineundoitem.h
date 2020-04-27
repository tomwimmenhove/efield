#ifndef DELETELINEUNDOITEM_H
#define DELETELINEUNDOITEM_H

#include <QSharedPointer>

#include "graphics/sceneelement.h"
#include "undostack.h"

class DeleteLineUndoItem : public UndoItem
{
public:
    DeleteLineUndoItem(const QSharedPointer<SceneElement<float>>& scene, int id);

    QString title() const override { return "Delete line"; }

    void undoFunction() override;
    void doFunction() override;

private:
    QSharedPointer<SceneElement<float>> scene;
    int id;
    int pointId1;
    int pointId2;
    float value;
};

#endif // DELETELINEUNDOITEM_H

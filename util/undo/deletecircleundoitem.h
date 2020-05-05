#ifndef DELETECIRCLEUNDOITEM_H
#define DELETECIRCLEUNDOITEM_H

#include <QSharedPointer>

#include "graphics/sceneelement.h"
#include "undostack.h"

class DeleteCircleUndoItem : public UndoItem
{
public:
    DeleteCircleUndoItem(const QSharedPointer<SceneElement<float>>& scene, int id);

    QString title() const override { return "Delete circle"; }

    void undoFunction() override;
    void doFunction() override;

private:
    QSharedPointer<SceneElement<float>> scene;
    int id;
    int pointId1;
    int pointId2;
    float value;
};

#endif // DELETECIRCLEUNDOITEM_H

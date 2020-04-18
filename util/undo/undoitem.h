#ifndef UNDOITEM_H
#define UNDOITEM_H

#include <QString>
#include <QSharedPointer>
#include <functional>

#include "graphics/sceneelement.h"

class UndoItem
{
public:
    UndoItem(const QSharedPointer<SceneElement<float>>& scene)
        : scene(scene)
    { }

    virtual QString title() const = 0;
    virtual void undoFunction() = 0;
    virtual void doFunction() = 0;

    virtual ~UndoItem() { }

protected:
    QSharedPointer<SceneElement<float>> scene;
};

#endif // UNDOITEM_H

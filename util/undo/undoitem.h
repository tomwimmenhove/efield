#ifndef UNDOITEM_H
#define UNDOITEM_H

#include <QString>
#include <QSharedPointer>
#include <functional>

#include "graphics/sceneelement.h"

class UndoItem
{
public:
    UndoItem(const QSharedPointer<SceneElement<float>>& scene, const QString& title)
        : scene(scene), titleString(title)
    { }

    inline QString title() const { return titleString; }
    virtual void undoFunction() = 0;
    virtual void doFunction() = 0;

    virtual ~UndoItem() { }

protected:
    QSharedPointer<SceneElement<float>> scene;
    QString titleString;
};

#endif // UNDOITEM_H

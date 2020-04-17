#ifndef LINEVALUEUNDOITEM_H
#define LINEVALUEUNDOITEM_H

#include <QSharedPointer>

#include "graphics/sceneelement.h"
#include "undostack.h"

class LineValueUndoItem : public UndoItem
{
public:
    LineValueUndoItem(const QSharedPointer<SceneElement<float>>& scene,
                      int id,
                      float oldValue,
                      float newValue,
                      const QString& title);

    void undoFunction() override;
    void doFunction() override;

private:
    int id;
    float oldValue;
    float newValue;
};
#endif // LINEVALUEUNDOITEM_H

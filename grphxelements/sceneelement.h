#ifndef SCENEELEMENT_H
#define SCENEELEMENT_H

#include "drawingelement.h"
#include "model/idrawer.h"

#include <QVector>
#include <QSharedPointer>

template<typename T>
class SceneElement : public DrawingElement<T>
{
public:
    SceneElement()
    { }

    void Add(DrawingElement<T>* element)
    {
        elements.push_back(QSharedPointer<DrawingElement<T>>(element));
    }

    void Clear() { elements.clear(); }

    void Draw(IDrawer<T>& drawer) override
    {
        for (auto const& element: elements)
            element->Draw(drawer);
    }

private:
    QVector<QSharedPointer<DrawingElement<T>>> elements;
};

#endif // SCENEELEMENT_H

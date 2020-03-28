#ifndef SCENEELEMENT_H
#define SCENEELEMENT_H

#include "drawingelement.h"
#include "graphics/idrawer.h"

#include <QVector>
#include <QSharedPointer>

template<typename T>
class SceneElement : public DrawingElement<T>
{
public:
    SceneElement()
    { }

    void Add(const QSharedPointer<DrawingElement<T>>& element)
    {
        elements.push_back(element);
    }

    int Remove(const QSharedPointer<DrawingElement<T>>& element)
    {
        return elements.removeAll(element);
    }

    void Clear() { elements.clear(); }

    void Draw(IDrawer<T>& drawer) override
    {
        for (auto const& element: elements)
            element->Draw(drawer);
    }

    void DrawAnnotation(QPainter& painter, const QSize& surfaceSize) override
    {
        for (auto const& element: elements)
            element->DrawAnnotation(painter, surfaceSize);
    }

private:
    QVector<QSharedPointer<DrawingElement<T>>> elements;
};

#endif // SCENEELEMENT_H

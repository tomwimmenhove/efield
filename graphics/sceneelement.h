#ifndef SCENEELEMENT_H
#define SCENEELEMENT_H

#include "drawingelement.h"
#include "graphics/idrawer.h"
#include "util/geometry.h"

#include <limits>
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
        for (auto const& e: elements)
            e->Draw(drawer);
    }

    void DrawAnnotation(QPainter& painter, const QSize& surfaceSize) override
    {
        for (auto const& e: elements)
            e->DrawAnnotation(painter, surfaceSize);
    }

    float DistanceTo(const QPoint&) const override
    {
        return 0.0f;
    }

    void HighLight(const QSharedPointer<DrawingElement<T>>& element)
    {
        for (auto const& e: elements)
            e->SetHighlighted(e == element);
    }

    QSharedPointer<DrawingElement<T>> ClosestElement(const QPoint& point,
                                                     float maxDist = std::numeric_limits<float>::max()) const
    {
        QSharedPointer<DrawingElement<T>> closest = nullptr;

        float min = std::numeric_limits<float>::max();
        for (auto const& element: elements)
        {
            float dist = element->DistanceTo(point);
            if (dist < min && dist < maxDist)
            {
                closest = element;
                min = dist;
            }
        }

        return closest;
    }

    void HighLightClosestElement(const QPoint& point,
                          float maxDist = std::numeric_limits<float>::max())
    {
        HighLight(ClosestElement(point, maxDist));
    }

private:
    QVector<QSharedPointer<DrawingElement<T>>> elements;
};

#endif // SCENEELEMENT_H

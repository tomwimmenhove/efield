#ifndef SCENEELEMENT_H
#define SCENEELEMENT_H

#include "drawingelement.h"
#include "graphics/idrawer.h"
#include "util/geometry.h"

#include <limits>
#include <QVector>
#include <QSharedPointer>
#include <QDomDocument>
#include <QUuid>
#include "nodeelement.h"

template<typename T>
class SceneElement : public DrawingElement<T>
{
public:
    SceneElement() { }

    virtual DrawingElementType ElementType() const { return DrawingElementType::Scene; }

    void Add(const QSharedPointer<DrawingElement<T>>& element)
    {
        elements.push_back(element);
    }

    int Remove(const QSharedPointer<DrawingElement<T>>& element)
    {
        return elements.removeAll(element);
    }

    void Clear() { elements.clear(); }

    const QVector<QSharedPointer<DrawingElement<T>>>& Elements() const { return elements; }

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

    void Highlight(const QSharedPointer<DrawingElement<T>>& element)
    {
        for (auto const& e: elements)
            e->SetHighlighted(e == element);
    }

    QSharedPointer<DrawingElement<T>> ClosestElement(const QPoint& point,
                                                     float maxDist = 15 /*std::numeric_limits<float>::max()*/,
                                                     DrawingElementType type = DrawingElementType::None) const
    {
        QSharedPointer<DrawingElement<T>> closest = nullptr;

        float min = std::numeric_limits<float>::max();
        for (auto const& e: elements)
        {
            if (type != DrawingElementType::None && type != e->ElementType())
                continue;

            float dist = e->DistanceTo(point);
            if (dist < min && dist < maxDist)
            {
                closest = e;
                min = dist;
            }
        }

        return closest;
    }

    void HighlightClosestElement(const QPoint& point,
                          float maxDist = 15 /*std::numeric_limits<float>::max()*/)
    {
        Highlight(ClosestElement(point, maxDist));
    }

    QSharedPointer<DrawingElement<T>> FindHighLigted() const
    {
        QSharedPointer<DrawingElement<T>> selected = nullptr;

        for (auto const& e: elements)
            if (e->IsHighlighted())
                return e;

        return selected;
    }

    void Accept(DrawingElementVisitor<T>& visitor) override { visitor.Visit(*this); }

private:
    QVector<QSharedPointer<DrawingElement<T>>> elements;
};

#endif // SCENEELEMENT_H

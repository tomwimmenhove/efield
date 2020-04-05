#ifndef SCENEELEMENT_H
#define SCENEELEMENT_H

#include "drawingelement.h"
#include "graphics/idrawer.h"
#include "util/geometry.h"

#include <limits>
#include <memory>
#include <QVector>
#include <QDomDocument>
#include <QUuid>
#include "nodeelement.h"

template<typename T>
class SceneElement : public DrawingElement<T>
{
public:
    SceneElement() { }

    virtual DrawingElementType ElementType() const { return DrawingElementType::Scene; }

    void Add(std::unique_ptr<DrawingElement<T>>&& element)
    {
        elements.push_back(std::move(element));
    }

    bool Remove(const DrawingElement<T>* element)
    {
        for (auto i = elements.begin(); i != elements.end(); ++i)
        {
            if (i->get() == element)
            {
                elements.erase(i);
                return true;
            }
        }
        return false;
    }

    void Clear() { elements.clear(); }

    const std::vector<std::unique_ptr<DrawingElement<T>>>& Elements() const { return elements; }

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

    void Highlight(const DrawingElement<T>* element)
    {
        for (auto const& e: elements)
            e->SetHighlighted(e.get() == element);
    }

    DrawingElement<T>* ClosestElement(const QPoint& point,
                                                     float maxDist = 15 /*std::numeric_limits<float>::max()*/,
                                                     DrawingElementType type = DrawingElementType::None) const
    {
        DrawingElement<T>* closest = nullptr;

        float min = std::numeric_limits<float>::max();
        for (auto const& e: elements)
        {
            if (type != DrawingElementType::None && type != e->ElementType())
                continue;

            float dist = e->DistanceTo(point);
            if (dist < min && dist < maxDist)
            {
                closest = e.get();
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

    DrawingElement<T>* FindHighLigted() const
    {
        for (auto const& e: elements)
            if (e->IsHighlighted())
                return e.get();

        return nullptr;
    }

    void Accept(DrawingElementVisitor<T>& visitor) override { visitor.Visit(*this); }

private:
    std::vector<std::unique_ptr<DrawingElement<T>>> elements;
};

#endif // SCENEELEMENT_H

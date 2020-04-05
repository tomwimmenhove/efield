#ifndef SCENEELEMENT_H
#define SCENEELEMENT_H

#include "drawingelement.h"
#include "graphics/idrawer.h"
#include "util/geometry.h"
#include "util/derefiterator.h"

#include <limits>
#include <memory>
#include <QVector>
#include <QDomDocument>
#include <QUuid>
#include "nodeelement.h"

template<typename T>
class SceneElement : public DrawingElement<T>
{
    using elementsIterator = typename std::vector<std::unique_ptr<DrawingElement<T>>>::const_iterator;

public:
    struct SceneIterator : public DerefIterator<elementsIterator>
    {
        SceneIterator(elementsIterator i) : DerefIterator<elementsIterator>(i) { }
    };

    SceneElement() { }

    virtual DrawingElementType ElementType() const { return DrawingElementType::Scene; }

    void Add(std::unique_ptr<DrawingElement<T>>&& element) { elements.push_back(std::move(element)); }
    void Remove(SceneIterator iter) { elements.erase(iter.innerIterator()); }
    void PopBack() { elements.pop_back(); }
    void Clear() { elements.clear(); }
    DrawingElement<T>& Front() const { return *elements.front(); }
    DrawingElement<T>& Back() const { return *elements.back(); }

    SceneIterator begin() const { return SceneIterator(elements.begin()); }
    SceneIterator end() const { return SceneIterator(elements.end()); }

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

    void Highlight(SceneIterator iter)
    {
        for (auto i = begin(); i != end(); ++i)
            i->SetHighlighted(i == iter);
    }

    void Highlight(const DrawingElement<T>& element)
    {
        for (auto const& e: elements)
            e->SetHighlighted(e.get() == &element);
    }

    SceneIterator ClosestElement(const QPoint& point, float maxDist = 15 /*std::numeric_limits<float>::max()*/,
                                 DrawingElementType type = DrawingElementType::None) const
    {
        SceneIterator closest = end();

        float min = std::numeric_limits<float>::max();
        for (auto i = begin(); i != end(); ++i)
        {
            if (type != DrawingElementType::None && type != i->ElementType())
                continue;

            float dist = i->DistanceTo(point);
            if (dist < min && dist < maxDist)
            {
                closest = i;
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

    SceneIterator FindHighLigted() const
    {
        return std::find_if(begin(), end(), [](const DrawingElement<T>& e) { return e.IsHighlighted(); });
    }

    void Accept(DrawingElementVisitor<T>& visitor) override { visitor.Visit(*this); }

private:
    std::vector<std::unique_ptr<DrawingElement<T>>> elements;
};

#endif // SCENEELEMENT_H

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
    struct SceneIterator : public DerefIterator<elementsIterator, DrawingElement<T>>
    {
        SceneIterator(elementsIterator i) : DerefIterator<elementsIterator, DrawingElement<T>>(i) { }
    };

    SceneElement() { }

    virtual drawingElementType elementType() const override { return drawingElementType::Scene; }

    void add(std::unique_ptr<DrawingElement<T>>&& element) { elements.push_back(std::move(element)); }
    void remove(SceneIterator iter) { elements.erase(iter.innerIterator()); }
    void pop() { elements.pop_back(); }
    void clear() { elements.clear(); }
    DrawingElement<T>& front() const { return *elements.front(); }
    DrawingElement<T>& back() const { return *elements.back(); }

    SceneIterator begin() const { return SceneIterator(elements.begin()); }
    SceneIterator end() const { return SceneIterator(elements.end()); }

    void draw(IDrawer<T>& drawer) override
    {
        for (auto const& e: elements)
            e->draw(drawer);
    }

    void drawAnnotation(QPainter& painter, const QSize& surfaceSize) override
    {
        for (auto const& e: elements)
            e->drawAnnotation(painter, surfaceSize);
    }

    float distanceTo(const QPoint&) const override
    {
        return 0.0f;
    }

    void highlight(SceneIterator iter)
    {
        for (auto i = begin(); i != end(); ++i)
            i->setHighlighted(i == iter);
    }

    SceneIterator closestElement(const QPoint& point, float maxDist = 15 /*std::numeric_limits<float>::max()*/,
                                 drawingElementType type = drawingElementType::None) const
    {
        SceneIterator closest = end();

        float min = std::numeric_limits<float>::max();
        for (auto i = begin(); i != end(); ++i)
        {
            if (type != drawingElementType::None && type != i->elementType())
                continue;

            float dist = i->distanceTo(point);
            if (dist < min && dist < maxDist)
            {
                closest = i;
                min = dist;
            }
        }

        return closest;
    }

    void highlightClosestElement(const QPoint& point,
                          float maxDist = 15 /*std::numeric_limits<float>::max()*/)
    {
        highlight(closestElement(point, maxDist));
    }

    SceneIterator findHighLighted() const
    {
        return std::find_if(begin(), end(), [](const DrawingElement<T>& e) { return e.isHighlighted(); });
    }

    void accept(DrawingElementVisitor<T>& visitor) override { visitor.visit(*this); }

private:
    std::vector<std::unique_ptr<DrawingElement<T>>> elements;
};

#endif // SCENEELEMENT_H

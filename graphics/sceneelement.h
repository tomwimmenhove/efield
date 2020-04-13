#ifndef SCENEELEMENT_H
#define SCENEELEMENT_H

#include "drawingelement.h"
#include "graphics/idrawer.h"
#include "util/geometry.h"
#include "util/derefiterator.h"
#include "nodeelement.h"

#include <limits>
#include <memory>
#include <QVector>
#include <QDomDocument>
#include <QUuid>
#include <functional>

template<typename T>
class SceneElement : public DrawingElement<T>
{
public:
    using iterator = DerefIterator<typename std::vector<std::unique_ptr<DrawingElement<T>>>::const_iterator, DrawingElement<T>>;

    SceneElement(int id = 0) : DrawingElement<T>(id), idCounter(id) { }

    inline int newId() { return ++idCounter; }
    inline void setInitialId(int id) { idCounter = id; }

    virtual drawingElementType elementType() const override { return drawingElementType::Scene; }

    void add(std::unique_ptr<DrawingElement<T>>&& element) { elements.push_back(std::move(element)); }
    void remove(iterator iter) { elements.erase(iter.innerIterator()); }
    void pop() { elements.pop_back(); }
    void clear() { elements.clear(); }
    DrawingElement<T>& front() const { return *elements.front(); }
    DrawingElement<T>& back() const { return *elements.back(); }

    iterator begin() const { return iterator(elements.begin()); }
    iterator end() const { return iterator(elements.end()); }

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

    void highlight(iterator iter)
    {
        for (auto i = begin(); i != end(); ++i)
            i->setHighlighted(i == iter);
    }

    iterator closestElement(const QPoint& point,
                            std::function<bool(const DrawingElement<T>&)> pred =
            [](const DrawingElement<T>&) { return true; }) const
    {
        iterator closest = end();

        float min = std::numeric_limits<float>::max();
        for (auto i = begin(); i != end(); ++i)
        {
            if (!pred(*i))
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

    void highlightClosestElement(const QPoint& point)
    {
        highlight(closestElement(point, maxDist));
    }

    iterator findHighLighted() const
    {
        return std::find_if(begin(), end(), [](const DrawingElement<T>& e) { return e.isHighlighted(); });
    }

    QPoint center() const override { return QPoint(); }
    bool canAnchor() const override { return false; }
    bool canDelete() const override { return false; }

    void accept(DrawingElementVisitor<T>& visitor) override { visitor.visit(*this); }

private:
    const float maxDist = 15;
    std::vector<std::unique_ptr<DrawingElement<T>>> elements;
    int idCounter;
};

#endif // SCENEELEMENT_H

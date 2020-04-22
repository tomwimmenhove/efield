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
#include <QRect>
#include <functional>

template<typename T>
class SceneElement : public DrawingElement<T>
{
public:
    using iterator = DerefIterator<typename std::vector<std::unique_ptr<DrawingElement<T>>>::const_iterator, DrawingElement<T>>;

    SceneElement(int id = 0)
        : DrawingElement<T>(id, QSize(0, 0)), idCounter(id)
    { }

    SceneElement(const QSize& size, int id = 0)
        : DrawingElement<T>(id, size), idCounter(id)
    { }

    inline int newId() { return ++idCounter; }
    inline void setInitialId(int id) { idCounter = id; }

    void add(std::unique_ptr<DrawingElement<T>>&& element)
    {
        Q_ASSERT(findId(element->identifier()) == end());
        elements.push_back(std::move(element));
    }
    iterator remove(iterator iter) { return iterator(elements.erase(iter.innerIterator())); }
    void pop() { elements.pop_back(); }
    void clear() { elements.clear(); }
    DrawingElement<T>& front() const { return *elements.front(); }
    DrawingElement<T>& back() const { return *elements.back(); }

    iterator begin() const { return iterator(elements.begin()); }
    iterator end() const { return iterator(elements.end()); }
    size_t size() const { return elements.size(); }

    void draw(IDrawer<T>& drawer) override
    {
        for (auto const& e: elements)
            e->draw(drawer);
    }

    void drawAnnotation(QPainter& painter, const QSize& surfaceSize) override
    {
        for (auto const& e: elements)
            e->drawAnnotation(painter, surfaceSize);

        if (!selectionRect.isNull())
        {
            QSize pSize = QSize(painter.device()->width(), painter.device()->height());
            QPoint sp1 = Geometry::scalePoint(selectionRect.topLeft(), surfaceSize, pSize);
            QPoint sp2 = Geometry::scalePoint(selectionRect.bottomRight(), surfaceSize, pSize);

            QRect flippedRect(QPoint(sp1.x(), painter.device()->height() - 1 - sp1.y()),
                              QPoint(sp2.x(), painter.device()->height() - 1 - sp2.y()));
            painter.setPen(Qt::magenta);
            painter.drawRect(flippedRect);
            painter.fillRect(flippedRect, QBrush(QColor(0xff, 0, 0, 0x20)));
        }
    }

    float distanceTo(const QPoint&) const override
    {
        return 0.0f;
    }

    void setSelectionRect(const QRect& rect) { selectionRect = rect; }

    void highlightExclusive(iterator iter)
    {
        for (auto i = begin(); i != end(); ++i)
            i->setHighlighted(i == iter);
    }

    void highlight(iterator iter)
    {
        iter->setHighlighted(true);
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
        highlightExclusive(closestElement(point, maxDist));
    }

    iterator findIf(const std::function<bool(const DrawingElement<T>&)>& pred) const
    {
        return std::find_if(begin(), end(), pred);
    }

    iterator findFirstHighLighted() const
    {
        return findIf([](const DrawingElement<T>& e) { return e.isHighlighted(); });
    }

    int numHighlighted() const
    {
        int n = 0;
        for (auto i = begin(); i != end(); ++i)
            if (i->isHighlighted())
                n++;

        return n;
    }

    iterator findId(int id) const
    {
        return findIf([id](const DrawingElement<T>& e) { return e.identifier() == id; });
    }

    const QRect selectionBounds()
    {
        QRect rect;

        for (auto i = begin(); i != end(); ++i)
            if (i->isHighlighted())
                rect = rect.isNull() ? i->bounds() : rect.united(i->bounds());

        return rect;
    }

    QPoint center() const override { return QPoint(); }
    bool setCenter(const QPoint&) override { return false; }
    QRect bounds() const override { return QRect(QPoint(0, 0), this->sceneBounds()); }
    bool canAnchor() const override { return false; }
    bool isInUse() const override { return true; }
    std::vector<int> uses() const override { return std::vector<int> { }; }
    QString name() const override { return "Scene"; }

    void accept(DrawingElementVisitor<T>& visitor) override { visitor.visit(*this); }

private:
    const float maxDist = 15;
    std::vector<std::unique_ptr<DrawingElement<T>>> elements;
    int idCounter;
    QRect selectionRect;
};

#endif // SCENEELEMENT_H

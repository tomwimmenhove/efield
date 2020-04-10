#ifndef NORMALMOUSEOPERATION_H
#define NORMALMOUSEOPERATION_H

#include "mouseoperation.h"
#include "dragnodemouseoperation.h"

class NormalMouseOperation : public MouseOperation
{
public:
    NormalMouseOperation(QSharedPointer<SceneElement<float>> scene) noexcept
        : MouseOperation(std::move(std::unique_ptr<MouseOperation>(nullptr)), scene)
    { }

    void activateOperation(std::unique_ptr<MouseOperation>& current, const QPoint& pointerPosition)
    {
        auto closest = scene->closestElement(pointerPosition);
        auto highLighted = scene->findHighLighted();

        if (highLighted != scene->end() && closest == highLighted &&
            highLighted->elementType() == drawingElementType::Node)
        {
            current = std::make_unique<DragNodeMouseOperation>(std::move(current), scene);
        }

        scene->highlight(closest);
    }

private:
    QPoint nodeSavedPos;
};


#endif // NORMALMOUSEOPERATION_H

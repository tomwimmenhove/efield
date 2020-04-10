#ifndef NORMALMOUSEOPERATION_H
#define NORMALMOUSEOPERATION_H

#include "mouseoperation.h"

class NormalMouseOperation : public MouseOperation
{
public:
    NormalMouseOperation(QSharedPointer<SceneElement<float>> scene) noexcept
        : MouseOperation(std::move(std::unique_ptr<MouseOperation>(nullptr)), scene)
    { }

    void mousePressed(std::unique_ptr<MouseOperation>& current, const QPoint& pointerPosition) override;

private:
    QPoint nodeSavedPos;
};

#endif // NORMALMOUSEOPERATION_H

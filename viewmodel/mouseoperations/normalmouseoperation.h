#ifndef NORMALMOUSEOPERATION_H
#define NORMALMOUSEOPERATION_H

#include <QObject>

#include "mouseoperation.h"
#include "graphics/nodeelement.h"
#include "graphics/lineelement.h"

class NormalMouseOperation : public QObject, public MouseOperation
{
    Q_OBJECT

public:
    NormalMouseOperation(const QSharedPointer<UndoStack>& undoStack,
                         const QSharedPointer<SceneElement<float>>& scene) noexcept
        : MouseOperation(std::move(std::unique_ptr<MouseOperation>(nullptr)), undoStack, scene)
    { }

    std::unique_ptr<MouseOperation> mousePressed(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition) override;
    std::unique_ptr<MouseOperation> mouseMoved(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition, Qt::MouseButtons buttons) override;
    std::unique_ptr<MouseOperation> mouseDoubleClicked(std::unique_ptr<MouseOperation>&& current, const QPoint& pointerPosition, Qt::MouseButtons buttons) override;

signals:
    void editElement(DrawingElement<float>& element);

private:
    bool checkStartedDragging(const QPoint& pointerPosition);
    bool startedDragging = false;
    QPoint mousePressedAt;
};

#endif // NORMALMOUSEOPERATION_H

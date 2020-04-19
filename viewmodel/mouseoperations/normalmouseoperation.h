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

    void mousePressed(std::unique_ptr<MouseOperation>& current, const QPoint& pointerPosition) override;
    void mouseMoved(std::unique_ptr<MouseOperation>& current, const QPoint& pointerPosition) override;
    void mouseReleased(std::unique_ptr<MouseOperation>& current, const QPoint& pointerPosition, Qt::MouseButtons buttons) override;
    void mouseDoubleClicked(std::unique_ptr<MouseOperation>& current, const QPoint& pointerPosition, Qt::MouseButtons buttons) override;

signals:
    void editElement(DrawingElement<float>& element);

private:
    bool pressed = false;
    QPoint mousePressedAt;
};

#endif // NORMALMOUSEOPERATION_H

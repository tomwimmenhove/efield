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
    NormalMouseOperation(QSharedPointer<SceneElement<float>> scene) noexcept
        : MouseOperation(std::move(std::unique_ptr<MouseOperation>(nullptr)), scene)
    { }

    void mousePressed(std::unique_ptr<MouseOperation>& current, const QPoint& pointerPosition) override;
    void mouseDoubleClicked(std::unique_ptr<MouseOperation>& current, const QPoint& pointerPosition, Qt::MouseButtons buttons) override;

signals:
    void editNode(NodeElement<float>& node);
    void editLine(LineElement<float>& line);

private:
    QPoint nodeSavedPos;
};

#endif // NORMALMOUSEOPERATION_H

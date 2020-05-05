#ifndef EDITDRAWINGELEMENTVISITOR_H
#define EDITDRAWINGELEMENTVISITOR_H

#include "graphics/lineelement.h"
#include "graphics/nodeelement.h"
#include "graphics/circleelement.h"
#include "graphics/sceneelement.h"

class EditDrawingElementVisitor : public QObject, public DrawingElementVisitor<float>
{
    Q_OBJECT

signals:
    void editLine(int id, float defaultValue);
    void editCircle(int id, float defaultValue);
    void editNode(int id, const QPoint& defaultPosition);

private:
    void visit(SceneElement<float>& scene) override;
    void visit(NodeElement<float>& node) override;
    void visit(LineElement<float>& line) override;
    void visit(CircleElement<float>&) override;
};

#endif // EDITDRAWINGELEMENTVISITOR_H

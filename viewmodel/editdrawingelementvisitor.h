#ifndef EDITDRAWINGELEMENTVISITOR_H
#define EDITDRAWINGELEMENTVISITOR_H

#include "graphics/lineelement.h"
#include "graphics/nodeelement.h"
#include "graphics/sceneelement.h"

class EditDrawingElementVisitor : public QObject, public DrawingElementVisitor<float>
{
    Q_OBJECT

signals:
    void editLine(int id, float defaultValue);
    void editNode(int id, QPoint defaultPosition);

private:
    void visit(SceneElement<float>& scene);
    void visit(NodeElement<float>& node);
    void visit(LineElement<float>& line);
};

#endif // EDITDRAWINGELEMENTVISITOR_H

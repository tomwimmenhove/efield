#include "editdrawingelementvisitor.h"

void EditDrawingElementVisitor::visit(SceneElement<float>&)
{ }

void EditDrawingElementVisitor::visit(NodeElement<float>& node)
{
    emit editNode(node.identifier(), node.anchorNode());
}

void EditDrawingElementVisitor::visit(LineElement<float>& line)
{
    emit editLine(line.identifier(), line.value());
}

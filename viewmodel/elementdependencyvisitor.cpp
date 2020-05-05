#include "elementdependencyvisitor.h"

void ElementDependencyVisitor::allHighlighted(SceneElement<float>& scene)
{
    for(auto& element: scene)
        if (element.isHighlighted())
            element.accept(*this);
}

void ElementDependencyVisitor::visit(SceneElement<float>& scene)
{
    for(auto& element: scene)
        element.accept(*this);
}

void ElementDependencyVisitor::visit(NodeElement<float>& node)
{
    deps.insert(node.identifier());
}

void ElementDependencyVisitor::visit(LineElement<float>& line)
{
    deps.insert(line.identifier());
    deps.insert(line.point1().identifier());
    deps.insert(line.point2().identifier());
}

void ElementDependencyVisitor::visit(CircleElement<float>& circle)
{
    deps.insert(circle.identifier());
    deps.insert(circle.point1().identifier());
    deps.insert(circle.point2().identifier());
}

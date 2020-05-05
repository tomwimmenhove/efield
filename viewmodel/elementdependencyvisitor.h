#ifndef ELEMENTDEPENDENCYVISITOR_H
#define ELEMENTDEPENDENCYVISITOR_H

#include <unordered_set>

#include <graphics/drawingelementvisitor.h>
#include <graphics/sceneelement.h>
#include <graphics/nodeelement.h>
#include <graphics/lineelement.h>
#include <graphics/circleelement.h>

class ElementDependencyVisitor : public DrawingElementVisitor<float>
{
public:
    ElementDependencyVisitor() { }

    void allHighlighted(SceneElement<float>& scene);

    const std::unordered_set<int>& dependencies() const { return deps; }

    void visit(SceneElement<float>& scene) override;
    void visit(NodeElement<float>& node) override;
    void visit(LineElement<float>& line) override;
    void visit(CircleElement<float>& circle) override;

private:
    std::unordered_set<int> deps;
};

#endif // ELEMENTDEPENDENCYVISITOR_H

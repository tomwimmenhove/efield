#ifndef DRAWINGELEMENTVISITOR_H
#define DRAWINGELEMENTVISITOR_H

template<typename T> class SceneElement;
template<typename T> class NodeElement;
template<typename T> class LineElement;
template<typename T> class CircleElement;

template<typename T>
class DrawingElementVisitor
{
public:
    virtual void visit(SceneElement<T>& element) = 0;
    virtual void visit(NodeElement<T>& element) = 0;
    virtual void visit(LineElement<T>& element) = 0;
    virtual void visit(CircleElement<T>& element) = 0;
};

#endif // DRAWINGELEMENTVISITOR_H

#ifndef DRAWINGELEMENTVISITOR_H
#define DRAWINGELEMENTVISITOR_H

template<typename T> class SceneElement;
template<typename T> class NodeElement;
template<typename T> class LineElement;

template<typename T>
class DrawingElementVisitor
{
public:
    virtual void Visit(SceneElement<T>& element) = 0;
    virtual void Visit(NodeElement<T>& element) = 0;
    virtual void Visit(LineElement<T>& element) = 0;
};

#endif // DRAWINGELEMENTVISITOR_H

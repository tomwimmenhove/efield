#ifndef COPYDRAWINGELEMENTVISITOR_H
#define COPYDRAWINGELEMENTVISITOR_H

#include "graphics/drawingelement.h"
#include "graphics/lineelement.h"
#include "graphics/nodeelement.h"
#include "graphics/sceneelement.h"
#include "util/undo/undostack.h"

#include <map>
#include <memory>

class CopyDrawingElementVisitor : public DrawingElementVisitor<float>
{
public:
    CopyDrawingElementVisitor(const QSharedPointer<SceneElement<float>>& source,
                              const QSharedPointer<SceneElement<float>>& destination,
                              const QSharedPointer<UndoStack>& undoStack = nullptr)
        : souce(source), destination(destination), undoStack(undoStack)
    { }

private:
    void visit(SceneElement<float>& scene) override;
    void visit(NodeElement<float>& node) override;
    void visit(LineElement<float>& line) override;
    void visit(CircleElement<float>& circle) override;

    template <typename T, typename U>
    void copyTwoNodeElement(T& element);

    QSharedPointer<SceneElement<float>> souce;
    QSharedPointer<SceneElement<float>> destination;
    QSharedPointer<UndoStack> undoStack;

    std::map<int, int> defaultMap;
    std::map<int, int>& idMap = defaultMap;

    int cloneNode(const SharedNode& point);

    CopyDrawingElementVisitor(const QSharedPointer<SceneElement<float>>& source,
                              const QSharedPointer<SceneElement<float>>& destination,
                              std::map<int, int>& idMap)
        : souce(source), destination(destination), idMap(idMap)
    { }
};

#endif // COPYDRAWINGELEMENTVISITOR_H

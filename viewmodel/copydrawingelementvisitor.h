#ifndef COPYDRAWINGELEMENTVISITOR_H
#define COPYDRAWINGELEMENTVISITOR_H

#include "graphics/drawingelement.h"
#include "graphics/lineelement.h"
#include "graphics/nodeelement.h"
#include "graphics/sceneelement.h"
//#include "model/floatsurface.h"
#include "util/undo/undostack.h"

#include <map>
#include <memory>

class CopyDrawingElementVisitor : public DrawingElementVisitor<float>
{
public:
    CopyDrawingElementVisitor(const QSharedPointer<SceneElement<float>>& scene,
                              const QSharedPointer<SceneElement<float>>& destination,
                              const QSharedPointer<UndoStack>& undoStack = nullptr)
        : scene(scene), destination(destination), undoStack(undoStack)
    { }

    void visit(SceneElement<float>& scene);
    void visit(NodeElement<float>& node);
    void visit(LineElement<float>& line);

private:
    QSharedPointer<SceneElement<float>> scene;
    QSharedPointer<SceneElement<float>> destination;
    QSharedPointer<UndoStack> undoStack;

    std::map<int, int> defaultMap;
    std::map<int, int>& idMap = defaultMap;

    int cloneNode(int originalId, const SharedNode& point);

    CopyDrawingElementVisitor(const QSharedPointer<SceneElement<float>>& scene,
                              const QSharedPointer<SceneElement<float>>& destination,
                              std::map<int, int>& idMap)
        : scene(scene), destination(destination), idMap(idMap)
    { }
};

#endif // COPYDRAWINGELEMENTVISITOR_H

#ifndef DELETEDRAWINGELEMENTVISITOR_H
#define DELETEDRAWINGELEMENTVISITOR_H

#include "graphics/drawingelement.h"
#include "graphics/lineelement.h"
#include "graphics/nodeelement.h"
#include "graphics/sceneelement.h"
#include "model/floatsurface.h"
#include "util/undo/undostack.h"

class DeleteDrawingElementVisitor : public DrawingElementVisitor<float>
{
public:
    static bool deleteElement(const QSharedPointer<UndoStack>& undoStack,
                     const QSharedPointer<SceneElement<float> >& scene,
                     DrawingElement<float>& element);

private:
    DeleteDrawingElementVisitor(const QSharedPointer<UndoStack>& undoStack,
                              const QSharedPointer<SceneElement<float>>& scene)
        : undoStack(undoStack), scene(scene)
    { }

    void visit(SceneElement<float>&);
    void visit(NodeElement<float>& node);
    void visit(LineElement<float>& line);

    bool update = false;
    QSharedPointer<UndoStack> undoStack;
    QSharedPointer<SceneElement<float>> scene;
    QSharedPointer<FloatSurface> surface;
};

#endif // DELETEDRAWINGELEMENTVISITOR_H

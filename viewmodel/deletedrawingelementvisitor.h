#ifndef DELETEDRAWINGELEMENTVISITOR_H
#define DELETEDRAWINGELEMENTVISITOR_H

#include "graphics/lineelement.h"
#include "graphics/nodeelement.h"
#include "graphics/circleelement.h"
#include "graphics/sceneelement.h"
#include "model/floatsurface.h"
#include "util/undo/undostack.h"

class DeleteDrawingElementVisitor : public DrawingElementVisitor<float>
{
public:
    DeleteDrawingElementVisitor(const QSharedPointer<UndoStack>& undoStack,
                              const QSharedPointer<SceneElement<float>>& scene)
        : undoStack(undoStack), scene(scene)
    { }

    bool needsUpdate() const { return update; }

private:
    void visit(SceneElement<float>&) override;
    void visit(NodeElement<float>& node) override;
    void visit(LineElement<float>& line) override;
    void visit(CircleElement<float>&) override;

    bool update = false;
    QSharedPointer<UndoStack> undoStack;
    QSharedPointer<SceneElement<float>> scene;
    QSharedPointer<FloatSurface> surface;
};

#endif // DELETEDRAWINGELEMENTVISITOR_H

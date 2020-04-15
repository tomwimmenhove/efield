#ifndef EDITDRAWINGELEMENTVISITOR_H
#define EDITDRAWINGELEMENTVISITOR_H

#include <QWidget>

#include "graphics/drawingelement.h"
#include "graphics/lineelement.h"
#include "graphics/nodeelement.h"
#include "graphics/sceneelement.h"
#include "model/floatsurface.h"
#include "util/undo/undostack.h"

class EditDrawingElementVisitor : public DrawingElementVisitor<float>
{
public:
    static bool editElement(QWidget* parentWidget,
                     const QSharedPointer<UndoStack>& undoStack,
                     const QSharedPointer<SceneElement<float> >& scene,
                     DrawingElement<float>& element,
                     const QSharedPointer<FloatSurface> surface);

private:
    EditDrawingElementVisitor(QWidget* parentWidget,
                              const QSharedPointer<UndoStack>& undoStack,
                              const QSharedPointer<SceneElement<float>>& scene,
                              QSharedPointer<FloatSurface> surface)
        : parentWidget(parentWidget), undoStack(undoStack), scene(scene), surface(surface)
    { }

    void visit(SceneElement<float>&);
    void visit(NodeElement<float>& node);
    void visit(LineElement<float>& line);

    bool update = false;
    QWidget* parentWidget;
    QSharedPointer<UndoStack> undoStack;
    QSharedPointer<SceneElement<float>> scene;
    QSharedPointer<FloatSurface> surface;
};

#endif // EDITDRAWINGELEMENTVISITOR_H

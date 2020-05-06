#ifndef ELEMENTMANIPULATORS_H
#define ELEMENTMANIPULATORS_H

#include <QSharedPointer>

#include "graphics/sceneelement.h"
#include "util/undo/undostack.h"

class ElementManipulators
{
public:
    ElementManipulators(const QSharedPointer<SceneElement<float>>& scene,
                        const QSharedPointer<UndoStack>& undoStack);

    void copySelection(QSharedPointer<SceneElement<float>> clipBoardScene, bool deleteAfter);
    bool paste(QSharedPointer<SceneElement<float>> clipBoardScene);

    void deleteSelected();
    bool rotateSelection(double angle);
    bool moveSelection(const QPoint& delta);

    bool needsUpdate() const { return update; }

private:
    QSharedPointer<SceneElement<float>> scene;
    QSharedPointer<UndoStack> undoStack;

    bool update = false;
};

#endif // ELEMENTMANIPULATORS_H

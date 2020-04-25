#ifndef COPYPASTE_H
#define COPYPASTE_H

#include <graphics/sceneelement.h>
#include <util/undo/undostack.h>

class CopyPaste
{
public:
    CopyPaste(const QSharedPointer<SceneElement<float>>& scene,
              const QSharedPointer<SceneElement<float>>& clipBoardScene,
              const QSharedPointer<UndoStack>& undoStack)
    : scene(scene), clipBoardScene(clipBoardScene), undoStack(undoStack)
    { }

    void copySelection(bool deleteAfter);
    bool pasteFits();
    void paste();

private:
    QSharedPointer<SceneElement<float>> scene;
    QSharedPointer<SceneElement<float>> clipBoardScene;
    QSharedPointer<UndoStack> undoStack;
};

#endif // COPYPASTE_H

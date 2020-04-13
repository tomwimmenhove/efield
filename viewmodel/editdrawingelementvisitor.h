#ifndef EDITDRAWINGELEMENTVISITOR_H
#define EDITDRAWINGELEMENTVISITOR_H

#include <QWidget>

#include <graphics/drawingelement.h>
#include <graphics/lineelement.h>
#include <graphics/nodeelement.h>
#include <graphics/sceneelement.h>
#include <model/floatsurface.h>

class EditDrawingElementVisitor : public DrawingElementVisitor<float>
{
public:
    static bool edit(QWidget* parentWidget, DrawingElement<float>& element, QSharedPointer<FloatSurface> surface);

private:
    EditDrawingElementVisitor(QWidget* parentWidget, QSharedPointer<FloatSurface> surface)
        : parentWidget(parentWidget), surface(surface)
    { }

    void visit(SceneElement<float>&);
    void visit(NodeElement<float>& node);
    void visit(LineElement<float>& line);

    bool update = false;
    QWidget* parentWidget;
    QSharedPointer<FloatSurface> surface;
};

#endif // EDITDRAWINGELEMENTVISITOR_H

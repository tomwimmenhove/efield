#include <QInputDialog>

#include "graphics/nodeelement.h"
#include "graphics/lineelement.h"
#include "normalmouseoperation.h"
#include "dragnodemouseoperation.h"
#include "pointinputdialog.h"

//class MouseOperationFactory : public DrawingElementVisitor<float>
//{
//public:
//    static inline std::unique_ptr<MouseOperation> getOperation(DrawingElement<float>& element, std::unique_ptr<MouseOperation>& current, QSharedPointer<SceneElement<float>> scene)
//    {
//        MouseOperationFactory f(current, scene);
//        element.accept(f);
//        return std::move(f.operation);
//    }

//private:
//    MouseOperationFactory(std::unique_ptr<MouseOperation>& current, QSharedPointer<SceneElement<float>> scene)
//        : current(current), scene(scene)
//    { }

//    void visit(SceneElement<float>&) { }
//    void visit(NodeElement<float>&) { operation = std::make_unique<DragNodeMouseOperation>(std::move(current), scene); }
//    void visit(LineElement<float>&) { }

//    std::unique_ptr<MouseOperation>& current;
//    QSharedPointer<SceneElement<float>> scene;
//    std::unique_ptr<MouseOperation> operation;
//};

//void NormalMouseOperation::mousePressed(std::unique_ptr<MouseOperation>& current, const QPoint& pointerPosition)
//{
//    auto closest = scene->closestElement(pointerPosition);
//    if (closest != scene->end())
//    {
//        auto newOperation = MouseOperationFactory::getOperation(*closest, current, scene);
//        if (newOperation)
//        {
//            current = std::move(newOperation);
//            current->activate(current, pointerPosition);
//            return;
//        }
//    }

//    scene->highlight(closest);

//    update();
//}

void NormalMouseOperation::mousePressed(std::unique_ptr<MouseOperation>& current, const QPoint& pointerPosition)
{
    auto closest = scene->closestElement(pointerPosition);
    if (closest != scene->end() && closest->elementType() == drawingElementType::Node)
    {
        current = std::make_unique<DragNodeMouseOperation>(std::move(current), scene);
        current->activate(current, pointerPosition);
        return;
    }

    scene->highlight(closest);

    update();
}

void NormalMouseOperation::mouseDoubleClicked(std::unique_ptr<MouseOperation>&, const QPoint& pointerPosition, Qt::MouseButtons buttons)
{
    if (buttons != Qt::LeftButton)
        return;

    auto closest = scene->closestElement(pointerPosition);
    if (closest != scene->end())
        emit editElement(*closest);
}

#include <QInputDialog>

#include <QDebug>

#include "editdrawingelementvisitor.h"
#include "pointinputdialog.h"
#include "util/undo/moveundoitem.h"
#include "util/undo/linevalueundoitem.h"

bool EditDrawingElementVisitor::editElement(QWidget* parentWidget, const QSharedPointer<UndoStack>& undoStack,
                                     const QSharedPointer<SceneElement<float>>& scene,
                                     DrawingElement<float>& element, const QSharedPointer<FloatSurface> surface)
{
    EditDrawingElementVisitor v(parentWidget, undoStack, scene, surface);
    element.accept(v);

    return v.update;
}

void EditDrawingElementVisitor::visit(SceneElement<float>&)
{ }

void EditDrawingElementVisitor::visit(NodeElement<float>& node)
{
    int id = node.anchorNode().identifier();
    qDebug() << "line id" << node.identifier() << ", sharedPoint id: " << id;

    SharedNode sharedNode = node.anchorNode();

    PointInputDialog d(QWidget::tr("Node coordinates"),
                       sharedNode, QPoint(0, 0), QPoint(surface->width() - 1, surface->height() - 1), parentWidget);
    if (d.exec() != QDialog::Accepted)
        return;

    MoveUndoItem undoItem(scene, node.identifier(), node.center(), d.point(), "Move");
    undoItem.doFunction();
    undoStack->add(undoItem);

    update = true;
}

void EditDrawingElementVisitor::visit(LineElement<float>& line)
{
    qDebug() << "line id" << line.identifier()
             << ", p1 id: " << line.point1().identifier()
             << ", p2 id: " << line.point2().identifier();

    bool ok;
    int def = line.value();

    int volt = QInputDialog::getInt(parentWidget, QWidget::tr("Edit line"),
                                    QWidget::tr("Voltage: "),  def, -2147483647, 2147483647, 1, &ok);
    if (!ok)
        return;

    LineValueUndoItem undoItem(scene, line.identifier(), def, volt, "Change voltage");
    undoStack->add(undoItem);
    undoItem.doFunction();
}

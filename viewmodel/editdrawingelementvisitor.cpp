#include <QInputDialog>

#include <QDebug>

#include "editdrawingelementvisitor.h"
#include <pointinputdialog.h>

bool EditDrawingElementVisitor::edit(QWidget* parentWidget, DrawingElement<float>& element, QSharedPointer<FloatSurface> surface)
{
    EditDrawingElementVisitor v(parentWidget, surface);
    element.accept(v);

    return v.update;
}

void EditDrawingElementVisitor::visit(SceneElement<float>&)
{ }

void EditDrawingElementVisitor::visit(NodeElement<float>& node)
{
    qDebug() << "line id" << node.identifier() << ", sharedPoint id: " << node.anchorNode().identifier();

    SharedNode sharedNode = node.anchorNode();

    PointInputDialog d(QWidget::tr("Node coordinates"),
                       sharedNode, QPoint(0, 0), QPoint(surface->width() - 1, surface->height() - 1), parentWidget);
    if (d.exec() != QDialog::Accepted)
        return;

    sharedNode.setPoint(d.point());

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
    if (ok)
        line.setValue(volt);
}

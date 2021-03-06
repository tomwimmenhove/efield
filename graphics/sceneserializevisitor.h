#ifndef SCENESERIALIZEVISITOR_H
#define SCENESERIALIZEVISITOR_H

#include <QDomElement>

#include "sceneelement.h"
#include "nodeelement.h"
#include "lineelement.h"
#include "circleelement.h"

template<typename T>
class SceneSerializeVisitor : public DrawingElementVisitor<T>
{
public:
    SceneSerializeVisitor(QDomElement& domElement, QDomDocument& domDocument)
        : domElement(domElement), domDocument(domDocument)
    { }

    void visit(SceneElement<T>& element) override
    {
        domElement.setAttribute("Width", QString::number(element.sceneSize().width()));
        domElement.setAttribute("Height", QString::number(element.sceneSize().height()));

        for (auto& e: element)
        {
            SceneSerializeVisitor visitor(domElement, domDocument);
            e.accept(visitor);
        }
    }

    void visit(NodeElement<T>& element) override
    {
        QDomElement node = domDocument.createElement("Node");
        node.setAttribute("X", QString::number(element.anchorNode().point().x()));
        node.setAttribute("Y", QString::number(element.anchorNode().point().y()));
        node.setAttribute("ID", QString::number(element.identifier()));
        domElement.appendChild(node);
    }

    void visit(LineElement<T>& element) override
    {
        visitTwoNodeElement<LineElement<T>>(element, "Line");
    }

    void visit(CircleElement<T>& element) override
    {
        visitTwoNodeElement<CircleElement<T>>(element, "Circle");
    }

private:
    template <typename U>
    void visitTwoNodeElement(U& element, const QString& name)
    {
        QDomElement line = domDocument.createElement(name);
        line.setAttribute("Node1", QString::number(element.point1().identifier()));
        line.setAttribute("Node2", QString::number(element.point2().identifier()));
        line.setAttribute("ID", QString::number(element.identifier()));
        line.appendChild(domDocument.createTextNode(valueToString(element.value())));
        domElement.appendChild(line);
    }

    /* This needs to be done for any type this template will be serialized to */
    inline QString valueToString(double x) const { return QString::number(x); }

    QDomElement& domElement;
    QDomDocument& domDocument;
};

#endif // SCENESERIALIZEVISITOR_H

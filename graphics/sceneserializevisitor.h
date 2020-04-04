#ifndef SCENESERIALIZEVISITOR_H
#define SCENESERIALIZEVISITOR_H

#include <QDomElement>

#include "sceneelement.h"
#include "nodeelement.h"
#include "lineelement.h"

template<typename T>
class SceneSerializeVisitor : public DrawingElementVisitor<T>
{
public:
    SceneSerializeVisitor(QDomElement& domElement, QDomDocument& domDocument)
        : domElement(domElement), domDocument(domDocument)
    { }

    void Visit(SceneElement<T>& element) override
    {
        for (const auto& e: element.Elements())
        {
            SceneSerializeVisitor visitor(domElement, domDocument);
            e->Accept(visitor);
        }
    }

    void Visit(NodeElement<T>& element) override
    {
        QDomElement node = domDocument.createElement("Node");
        node.setAttribute("X", QString::number(element.Node().Point().x()));
        node.setAttribute("Y", QString::number(element.Node().Point().y()));
        node.setAttribute("ID", QString::number(element.Node()->GetId()));
        domElement.appendChild(node);
    }

    void Visit(LineElement<T>& element) override
    {
        QDomElement line = domDocument.createElement("Line");
        line.setAttribute("Node1", QString::number(element.P1()->GetId()));
        line.setAttribute("Node2", QString::number(element.P2()->GetId()));
        line.appendChild(domDocument.createTextNode(ValueToString(element.Value())));
        domElement.appendChild(line);
    }

private:
    /* This needs to be done for any type this template will be serialized to */
    inline QString ValueToString(double x) const { return QString::number(x); }

    QDomElement& domElement;
    QDomDocument& domDocument;
};

#endif // SCENESERIALIZEVISITOR_H

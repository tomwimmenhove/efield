#ifndef SCENEDESERIALIZEVISITOR_H
#define SCENEDESERIALIZEVISITOR_H

#include <functional>
#include <array>
#include <memory>
#include <QDomElement>
#include <QPair>
#include <QMap>

#include "sceneelement.h"
#include "nodeelement.h"
#include "lineelement.h"

template<typename T>
class SceneDeserializeVisitor : public DrawingElementVisitor<T>
{
public:
    SceneDeserializeVisitor(QDomElement& domElement)
        : domElement(domElement)
    { }

    void visit(SceneElement<T>& element) override
    {
        std::array<QPair<QString, std::function<DrawingElement<T>*(int id, const QSize& bounds)>>, 2> nodeFactories
        {{
            { "Node", [](int id, const QSize& bounds) { return new NodeElement<T>(id, bounds); } }, // First, because LineElements depends on it
            { "Line", [](int id, const QSize& bounds) { return new LineElement<T>(id, bounds); } },
        }};

        int maxId = 0;

        QDomNamedNodeMap attributes = domElement.attributes();
        QSize size(attributes.namedItem("Width").nodeValue().toInt(),
                   attributes.namedItem("Height").nodeValue().toInt());
        element.setBounds(size);

        for(auto& nodeFactory: nodeFactories)
        {
            auto nodeXmlElements = domElement.elementsByTagName(nodeFactory.first);
            for(int i = 0; i < nodeXmlElements.count(); i++)
            {
                QDomElement nodeXmlElement = nodeXmlElements.item(i).toElement();
                QDomNamedNodeMap attributes = nodeXmlElement.attributes();

                int id = attributes.namedItem("ID").nodeValue().toInt();
                if (id > maxId)
                    maxId = id;

                std::unique_ptr<DrawingElement<T>> nodeElement = std::unique_ptr<DrawingElement<T>>(nodeFactory.second(id, size));

                SceneDeserializeVisitor visitor(nodeXmlElement, nodeMap);
                nodeElement->accept(visitor);
                element.add(std::move(nodeElement));
            }
        }

        element.setInitialId(maxId);
    }

    void visit(NodeElement<T>& element) override
    {
        QDomNamedNodeMap attributes = domElement.attributes();
        SharedNode sharedNode(element.identifier(),
                              attributes.namedItem("X").nodeValue().toInt(),
                              attributes.namedItem("Y").nodeValue().toInt());
        nodeMap[element.identifier()] = sharedNode;
        element.setNode(sharedNode);
    }

    void visit(LineElement<T>& element) override
    {
        QDomNamedNodeMap attributes = domElement.attributes();
        float value;
        stringToValue(value, domElement.firstChild().nodeValue());

        element.setValue(value);
        element.setPoint1(nodeMap[attributes.namedItem("Node1").nodeValue().toInt()]);
        element.setPoint2(nodeMap[attributes.namedItem("Node2").nodeValue().toInt()]);
    }

private:
    QMap<int, SharedNode> defaultMap;
    QDomElement& domElement;
    QMap<int, SharedNode>& nodeMap = defaultMap;

    SceneDeserializeVisitor(QDomElement& domElement, QMap<int, SharedNode>& nodeMap)
        : domElement(domElement), nodeMap(nodeMap)
    { }

    /* This needs to be done for any type this template will be serialized to */
    inline void stringToValue(T& result, const QString& s) { result = s.toDouble(); }
};

#endif // SCENEDESERIALIZEVISITOR_H

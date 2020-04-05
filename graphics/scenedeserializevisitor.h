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

    void Visit(SceneElement<T>& element) override
    {
        std::array<QPair<QString, std::function<DrawingElement<T>*(void)>>, 2> nodeFactories
        {{
            { "Node", []() { return new NodeElement<T>(); } }, // First, because LineElements depends on it
            { "Line", []() { return new LineElement<T>(); } },
        }};

        for(auto& nodeFactory: nodeFactories)
        {
            auto nodeXmlElements = domElement.elementsByTagName(nodeFactory.first);
            for(int i = 0; i < nodeXmlElements.count(); i++)
            {
                std::unique_ptr<DrawingElement<T>> nodeElement = std::unique_ptr<DrawingElement<T>>(nodeFactory.second());
                QDomElement nodeXmlElement = nodeXmlElements.item(i).toElement();
                SceneDeserializeVisitor visitor(nodeXmlElement, nodeMap);
                nodeElement->Accept(visitor);
                element.Add(std::move(nodeElement));
            }
        }
    }

    void Visit(NodeElement<T>& element) override
    {
        QDomNamedNodeMap attributes = domElement.attributes();
        SharedNode sharedNode(attributes.namedItem("X").nodeValue().toInt(),
                              attributes.namedItem("Y").nodeValue().toInt());
        int id = attributes.namedItem("ID").nodeValue().toInt();
        nodeMap[id] = sharedNode;
        element.SetNode(sharedNode);
    }

    void Visit(LineElement<T>& element) override
    {
        QDomNamedNodeMap attributes = domElement.attributes();
        float value;
        StringToValue(value, domElement.firstChild().nodeValue());

        element.SetValue(value);
        element.SetP1(nodeMap[attributes.namedItem("Node1").nodeValue().toInt()]);
        element.SetP2(nodeMap[attributes.namedItem("Node2").nodeValue().toInt()]);
    }

private:
    QMap<int, SharedNode> defaultMap;
    QDomElement& domElement;
    QMap<int, SharedNode>& nodeMap = defaultMap;

    SceneDeserializeVisitor(QDomElement& domElement, QMap<int, SharedNode>& nodeMap)
        : domElement(domElement), nodeMap(nodeMap)
    { }

    /* This needs to be done for any type this template will be serialized to */
    inline void StringToValue(T& result, const QString& s) { result = s.toDouble(); }
};

#endif // SCENEDESERIALIZEVISITOR_H

#include "project.h"

Project::Project(const QSize& size)
{
    allocate(size);
}

Project::Project(const QString& xml)
{
    QDomDocument doc;
    doc.setContent(xml);
    fromDoc(doc);
}

Project::Project(const QByteArray& xml)
{
    QDomDocument doc;
    doc.setContent(xml);
    fromDoc(doc);
}

void Project::allocate(const QSize& size)
{
    scene = QSharedPointer<SceneElement<float>>::create();
    simulator = QSharedPointer<Simulator>::create(size,
        std::bind(&Project::setFixedValues, this, std::placeholders::_1));
}

void Project::fromDoc(const QDomDocument& doc)
{
    QDomElement root = doc.documentElement();

    QDomNode sizeXmlElement = root.elementsByTagName("Size").item(0);
    QDomNamedNodeMap attributes = sizeXmlElement.attributes();
    QSize size(attributes.namedItem("Width").nodeValue().toInt(),
               attributes.namedItem("Height").nodeValue().toInt());

    allocate(size);

    QDomElement sceneXmlElement = root.elementsByTagName("Scene").item(0).toElement();
    SceneDeserializeVisitor<float> visitor(sceneXmlElement);

    visitor.visit(*scene);
}

QDomDocument Project::toDoc()
{
    QDomDocument doc("EFieldSim");
    QDomElement root = doc.createElement("EFieldSim");
    doc.appendChild(root);

    QDomElement sizXmleElement = doc.createElement("Size");
    sizXmleElement.setAttribute("Width", QString::number(simulator->size().width()));
    sizXmleElement.setAttribute("Height", QString::number(simulator->size().height()));
    root.appendChild(sizXmleElement);

    QDomElement sceneXmlElement = doc.createElement("Scene");
    root.appendChild(sceneXmlElement);

    SceneSerializeVisitor<float> visitor(sceneXmlElement, doc);

    visitor.visit(*scene);

    return doc;
}

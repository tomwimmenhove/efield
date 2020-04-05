#include "project.h"

Project::Project(const QSize& size)
{
    simulator = QSharedPointer<Simulator>::create(size, updateBoundariesHandler);
}

Project::Project(const QString& xml)
{
    QDomDocument doc;
    doc.setContent(xml);
    FromDoc(doc);
}

Project::Project(const QByteArray& xml)
{
    QDomDocument doc;
    doc.setContent(xml);
    FromDoc(doc);
}

void Project::FromDoc(const QDomDocument& doc)
{
    QDomElement root = doc.documentElement();

    QDomNode sizeXmlElement = root.elementsByTagName("Size").item(0);
    QDomNamedNodeMap attributes = sizeXmlElement.attributes();
    QSize size(attributes.namedItem("Width").nodeValue().toInt(),
               attributes.namedItem("Height").nodeValue().toInt());

    simulator = QSharedPointer<Simulator>::create(size, updateBoundariesHandler);

    QDomElement sceneXmlElement = root.elementsByTagName("Scene").item(0).toElement();
    SceneDeserializeVisitor<float> visitor(sceneXmlElement);

    visitor.Visit(scene);
}

QDomDocument Project::ToDoc()
{
    QDomDocument doc("EFieldSim");
    QDomElement root = doc.createElement("EFieldSim");
    doc.appendChild(root);

    QDomElement sizXmleElement = doc.createElement("Size");
    sizXmleElement.setAttribute("Width", QString::number(simulator->Size().width()));
    sizXmleElement.setAttribute("Height", QString::number(simulator->Size().height()));
    root.appendChild(sizXmleElement);

    QDomElement sceneXmlElement = doc.createElement("Scene");
    root.appendChild(sceneXmlElement);

    SceneSerializeVisitor<float> visitor(sceneXmlElement, doc);
    visitor.Visit(scene);

    return doc;
}

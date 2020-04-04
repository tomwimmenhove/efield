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

    QDomNode sizeElement = root.elementsByTagName("Size").item(0);
    QDomNamedNodeMap attributes = sizeElement.attributes();
    QSize size(attributes.namedItem("Width").nodeValue().toInt(),
               attributes.namedItem("Height").nodeValue().toInt());

    simulator = QSharedPointer<Simulator>::create(size, updateBoundariesHandler);

    QDomElement sim = root.elementsByTagName("Scene").item(0).toElement();
    SceneDeserializeVisitor<float> visitor(sim);

    visitor.Visit(scene);
}

QDomDocument Project::ToDoc()
{
    QDomDocument doc("EFieldSim");
    QDomElement root = doc.createElement("EFieldSim");
    doc.appendChild(root);

    QDomElement sizeElement = doc.createElement("Size");
    sizeElement.setAttribute("Width", QString::number(simulator->Size().width()));
    sizeElement.setAttribute("Height", QString::number(simulator->Size().height()));
    root.appendChild(sizeElement);

    QDomElement sceneXmlElement = doc.createElement("Scene");
    root.appendChild(sceneXmlElement);

    SceneSerializeVisitor<float> testSerializeVisitor(sceneXmlElement, doc);
    testSerializeVisitor.Visit(scene);

    return doc;
}

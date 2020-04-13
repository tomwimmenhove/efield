#include "project.h"

Project::Project(const QSize& size)
{
    scene = QSharedPointer<SceneElement<float>>::create(size);
    simulator = QSharedPointer<Simulator>::create(size,
        std::bind(&Project::setFixedValues, this, std::placeholders::_1));
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

void Project::fromDoc(const QDomDocument& doc)
{
    QDomElement root = doc.documentElement();

    QDomElement sceneXmlElement = root.elementsByTagName("Scene").item(0).toElement();
    SceneDeserializeVisitor<float> visitor(sceneXmlElement);

    scene = QSharedPointer<SceneElement<float>>::create();
    visitor.visit(*scene);

    simulator = QSharedPointer<Simulator>::create(scene->size(),
        std::bind(&Project::setFixedValues, this, std::placeholders::_1));
}

QDomDocument Project::toDoc()
{
    QDomDocument doc("EFieldSim");
    QDomElement root = doc.createElement("EFieldSim");
    doc.appendChild(root);

    QDomElement sceneXmlElement = doc.createElement("Scene");
    root.appendChild(sceneXmlElement);

    SceneSerializeVisitor<float> visitor(sceneXmlElement, doc);

    visitor.visit(*scene);

    return doc;
}

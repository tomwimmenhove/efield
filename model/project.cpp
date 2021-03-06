#include "project.h"

Project::Project(const QSize& size)
{
    sharedScene = QSharedPointer<SceneElement<float>>::create(size);
    sharedSimulator = QSharedPointer<Simulator>::create(size,
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

QString Project::fileName() const
{
    return fName;
}

void Project::setFileName(const QString& value)
{
    fName = value;
    sendStatusUpdate();
}

bool Project::isAltered() const
{
    return altered;
}

void Project::setAltered(bool value)
{
    altered = value;
    sendStatusUpdate();
}

void Project::sendStatusUpdate()
{
    emit statusUpdate(fName, altered);
}

void Project::fromDoc(const QDomDocument& doc)
{
    QDomElement root = doc.documentElement();

    QDomElement sceneXmlElement = root.elementsByTagName("Scene").item(0).toElement();
    SceneDeserializeVisitor<float> visitor(sceneXmlElement);

    sharedScene = QSharedPointer<SceneElement<float>>::create();
    visitor.visit(*sharedScene);

    sharedSimulator = QSharedPointer<Simulator>::create(sharedScene->sceneSize(),
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

    visitor.visit(*sharedScene);

    return doc;
}

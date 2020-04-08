#ifndef PROJECT_H
#define PROJECT_H

#include <QSize>
#include <QDomDocument>
#include <QSharedPointer>

#include "simulator.h"
#include "floatsurfacedrawer.h"
#include "graphics/sceneserializevisitor.h"
#include "graphics/scenedeserializevisitor.h"

class Project
{
public:
    Project(const QSize& size);

    /* Load */
    Project(const QString& xml);
    Project(const QByteArray& xml);

    /* Save */
    QString toXmlString() { return toDoc().toString(4); }
    QByteArray toXmlBytes() { return toDoc().toByteArray(4); }

    SceneElement<float>& sceneRef() { return scene; }
    QSharedPointer<Simulator> sharedSimulator() { return simulator; }

private:
    const std::function<void (FloatSurface&)> updateBoundariesHandler =
            std::bind(&Project::setFixedValues, this, std::placeholders::_1);

    void fromDoc(const QDomDocument& doc);
    QDomDocument toDoc();

    void setFixedValues(FloatSurface& surface)
    {
        FloatSurfaceDrawer drawer(surface);
        scene.draw(drawer);
    }

    SceneElement<float> scene;
    QSharedPointer<Simulator> simulator;
};

#endif // PROJECT_H

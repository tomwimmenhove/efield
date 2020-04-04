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
    QString ToXmlString() { return ToDoc().toString(4); }
    QByteArray ToXmlBytes() { return ToDoc().toByteArray(4); }

    SceneElement<float>& SceneRef() { return scene; }
    QSharedPointer<Simulator> SharedSimulator() { return simulator; }

private:
    const std::function<void (FloatSurface&)> updateBoundariesHandler =
            std::bind(&Project::SetFixedValues, this, std::placeholders::_1);

    void FromDoc(const QDomDocument& doc);
    QDomDocument ToDoc();

    void SetFixedValues(FloatSurface& surface)
    {
        FloatSurfaceDrawer drawer(surface);
        scene.Draw(drawer);
    }

    SceneElement<float> scene;
    QSharedPointer<Simulator> simulator;
};

#endif // PROJECT_H

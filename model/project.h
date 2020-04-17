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

    QSharedPointer<SceneElement<float>> scene() const { return sharedScene; }
    QSharedPointer<Simulator> simulator() const { return sharedSimulator; }

private:
    void fromDoc(const QDomDocument& doc);
    QDomDocument toDoc();

    void setFixedValues(FloatSurface& surface)
    {
        FloatSurfaceDrawer drawer(surface);
        sharedScene->draw(drawer);
    }

    QSharedPointer<SceneElement<float>> sharedScene;
    QSharedPointer<Simulator> sharedSimulator;
};

#endif // PROJECT_H

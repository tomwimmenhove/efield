#ifndef PROJECT_H
#define PROJECT_H

#include <QSize>
#include <QDomDocument>
#include <QSharedPointer>
#include <QObject>

#include "simulator.h"
#include "floatsurfacedrawer.h"
#include "graphics/sceneserializevisitor.h"
#include "graphics/scenedeserializevisitor.h"

class Project : public QObject
{
    Q_OBJECT

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

    QString fileName() const;
    void setFileName(const QString& value);

    bool isAltered() const;
    void setAltered(bool value);

    void sendStatusUpdate();

signals:
    void statusUpdate(const QString& filename, bool altered);

private:
    void fromDoc(const QDomDocument& doc);
    QDomDocument toDoc();

    void setFixedValues(FloatSurface& surface)
    {
        FloatSurfaceDrawer drawer(surface);
        sharedScene->draw(drawer);
    }

    QString fName;
    bool altered = false;
    QSharedPointer<SceneElement<float>> sharedScene;
    QSharedPointer<Simulator> sharedSimulator;
};

#endif // PROJECT_H

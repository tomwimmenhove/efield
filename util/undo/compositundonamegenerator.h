#ifndef COMPOSITUNDONAMEGENERATOR_H
#define COMPOSITUNDONAMEGENERATOR_H

#include <QMap>
#include <QString>

class CompositUndoNameGenerator
{
public:
    CompositUndoNameGenerator(const QString& operationName);

    void addElementName(const QString& name);
    QString generate();

private:
    QString operationName;
    QMap<QString, int> namesMap;
};

#endif // COMPOSITUNDONAMEGENERATOR_H

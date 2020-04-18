#include "compositundonamegenerator.h"

CompositUndoNameGenerator::CompositUndoNameGenerator(const QString& operationName)
    : operationName(operationName)
{ }

void CompositUndoNameGenerator::addElementName(const QString& name)
{
    namesMap[name]++;
}

QString CompositUndoNameGenerator::generate()
{
    QString name = operationName + " ";
    if (namesMap.size() == 1 && namesMap.first() == 1)
        name += namesMap.firstKey();
    else
    {
        QStringList list;

        QMapIterator<QString, int> i(namesMap);
        while (i.hasNext())
        {
            i.next();
            list += QString("%1(%2)").arg(i.key()).arg(i.value());
        }

        name += list.join(", ");
    }

    return name;
}

#ifndef SIMULATORTHREAD_H
#define SIMULATORTHREAD_H

#include <QThread>
#include <QAtomicInt>
#include <QMutex>

#include "simulator.h"

class SimulatorThread : public QThread
{
    Q_OBJECT

public:
    SimulatorThread(QSharedPointer<Simulator> simulator);

    void run() override;
    inline void Cancel() { cancel = true; }
    inline void RequestSurface() { requestSurface++; }
    
signals:
    void NewSurface(QSharedPointer<Surface> surface);

private:
    QMutex mutex;
    QSharedPointer<Simulator> simulator;

    QAtomicInt cancel = false;
    QAtomicInt requestSurface = false;
};

Q_DECLARE_METATYPE(QSharedPointer<Surface>);

#endif // SIMULATORTHREAD_H

#ifndef SIMULATORTHREAD_H
#define SIMULATORTHREAD_H

#include <QThread>
#include <QAtomicInt>
#include <QMutex>
#include <QDebug>

#include "simulator.h"

class SimulatorThread : public QThread
{
    Q_OBJECT

public:
    SimulatorThread(QSharedPointer<Simulator> simulator)
        : simulator(simulator)
    { }

    virtual ~SimulatorThread()
    {
        qDebug() << "Bubbye";
    }

#ifdef _OPENMP
    inline void SetNumThreads(int n) { numThreads = n; }
    inline int NumThreads() const { return numThreads; }

    void RunParallel();
#endif
    void RunSequential();

    void run() override;
    inline void Cancel() { cancel = true; }
    inline int Iterations() const { return iterations; }
    
private:
    int numThreads = 1;
    QAtomicInt iterations = 0;
    QMutex mutex;
    QSharedPointer<Simulator> simulator;

    QAtomicInt cancel = false;
};

#endif // SIMULATORTHREAD_H

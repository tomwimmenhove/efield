#ifndef SIMULATORWORKER_H
#define SIMULATORWORKER_H

#include <QObject>
#include <QAtomicInt>
#include <QMutex>
#include <QDebug>

#include "simulator.h"

class SimulatorWorker : public QObject
{
    Q_OBJECT
public:
    explicit SimulatorWorker(QSharedPointer<Simulator> simulator, QObject *parent = nullptr)
        : QObject(parent), simulator(simulator)
    { }

#ifdef _OPENMP
    inline void SetNumThreads(int n) { numThreads = n; }
    inline int NumThreads() const { return numThreads; }
#endif

    inline int Iterations() const { return iterations; }

signals:

public slots:
    void Run();
    inline void Cancel() { cancel = true; }

private:
#ifdef _OPENMP
    void RunParallel();
#endif
    void RunSequential();

    int numThreads = 1;
    QAtomicInt iterations = 0;
    QMutex mutex;
    QSharedPointer<Simulator> simulator;

    QAtomicInt cancel = false;
};

#endif // SIMULATORWORKER_H

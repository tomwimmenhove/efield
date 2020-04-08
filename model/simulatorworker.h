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
    inline void setNumThreads(int n) { nThreads = n; }
    inline int numThreads() const { return nThreads; }
#endif

    inline int iterations() const { return iters; }

signals:

public slots:
    void run();
    inline void stop() { cancel = true; }

private:
#ifdef _OPENMP
    void runParallel();
#endif
    void runSequential();

    int nThreads = 1;
    QAtomicInt iters = 0;
    QMutex mutex;
    QSharedPointer<Simulator> simulator;

    QAtomicInt cancel = false;
};

#endif // SIMULATORWORKER_H

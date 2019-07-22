#ifndef LEASTSQUAREMETHOD_H
#define LEASTSQUAREMETHOD_H

#include "qcustomplot.h"
#include "status.h"

#include <QObject>

class LeastSquareMethod : public QObject
{
    Q_OBJECT
public:
    explicit LeastSquareMethod(QObject *parent = nullptr);

signals:
    void statusChanged(Status status, QString message = "") const;

public slots:
    void doWork(int windowSize, int shift, QCPGraph *graph);
private:
    QCPGraph *graph;
};

#endif // LEASTSQUAREMETHOD_H

#ifndef LEASTSQUAREMETHOD_H
#define LEASTSQUAREMETHOD_H

#include "graph.h"
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
    void createGraph(const Graph &graph, QCPGraph::LineStyle lineStyle);
    void portionApproximated(const QList<Graph> &graphs) const;


public slots:
    void doWork(int windowSize, int shift, QCPGraph *graphResult);
    void resultGraphChangedName(const QString &oldName, const QString &newName);
private:

    void calculateWindow(int shift, int windowSize, QCPGraph *graph);
    void kramer(const QMap<QString,double> &summ, double xMid);
    void doEmit();
    Graph graphResult;
    QList<double> xResult;
    QList<double> yResult;
};

#endif // LEASTSQUAREMETHOD_H

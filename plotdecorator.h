#ifndef PLOTDECORATOR_H
#define PLOTDECORATOR_H

#include "graph.h"
#include "qcustomplot.h"

#include <QObject>

class PlotDecorator : public QObject
{
    Q_OBJECT
public:
    explicit PlotDecorator(QCustomPlot *plot, QObject *parent = nullptr);

signals:

public slots:
    void drawPortion(const QList<Graph> &graphs);

private slots:
    void onXRangeChanged(const QCPRange &range);
    void onYRangeChanged(const QCPRange &range);

private:

    void updateRange(const QList<Graph> &graphs);
    void init(const Graph &graph);


    bool wasInitiated;
    QCustomPlot *plot;


    double minX;
    double maxX;

    double minY;
    double maxY;
};

#endif // PLOTDECORATOR_H

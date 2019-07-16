#include "plotdecorator.h"

PlotDecorator::PlotDecorator(QCustomPlot *plot,QObject *parent)
    : QObject(parent), wasInitiated(false), plot(plot)
{
    connect(plot->xAxis, QOverload<const QCPRange &>::of(&QCPAxis::rangeChanged),
            this, &PlotDecorator::onXRangeChanged);
    connect(plot->yAxis, QOverload<const QCPRange &>::of(&QCPAxis::rangeChanged),
            this,&PlotDecorator::onYRangeChanged);

}

void PlotDecorator::onXRangeChanged(const QCPRange &range)
{
    QCPRange boundedRange = range;

    if(boundedRange.lower < minX)
        boundedRange.lower = minX;

    if (boundedRange.upper > maxX)
        boundedRange.upper = maxX;

    plot->xAxis->setRange(boundedRange);
}

void PlotDecorator::onYRangeChanged(const QCPRange &range)
{
    QCPRange boundedRange = range;

    if(boundedRange.lower < minY)
        boundedRange.lower = minY;

    if (boundedRange.upper > maxY)
        boundedRange.upper = maxY;

    plot->yAxis->setRange(boundedRange);
}

void PlotDecorator::drawPortion(const QList<Graph> &graphs)
{
    qApp->processEvents();
    for (int i = 0; i < graphs.count(); i++)
    {
        const Graph graph = graphs.at(i);
        plot->graph(i)->addData(graph.getX(),graph.getY());
    }
    updateRange(graphs);
    plot->replot();
    qApp->processEvents();
}

void PlotDecorator::updateRange(const QList<Graph> &graphs)
{
    if (graphs.count() == 0)
        return;

    if (!wasInitiated)
    {
        wasInitiated = true;
        init(graphs.at(0));
    }

    for (const Graph &graph : graphs) {
        minX = qMin(minX,graph.getMinX());
        maxX = qMax(maxX,graph.getMaxX());

        minY = qMin(minY,graph.getMinY());
        maxY = qMax(maxY,graph.getMaxY());
    }

    plot->xAxis->setRange(minX,maxX);
    plot->yAxis->setRange(minY,maxY);
}

void PlotDecorator::init(const Graph &graph)
{
    minX = graph.getMinX();
    maxX = graph.getMaxX();

    minY = graph.getMinY();
    maxY = graph.getMaxY();
}

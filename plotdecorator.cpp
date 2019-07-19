#include "plotdecorator.h"

PlotDecorator::PlotDecorator(QCustomPlot *plot,QObject *parent)
    : QObject(parent), wasInitiated(false), plot(plot),
      draggingLegend(false)
{
    connect(plot->xAxis, QOverload<const QCPRange &>::of(&QCPAxis::rangeChanged),
            this, &PlotDecorator::onXRangeChanged);
    connect(plot->yAxis, QOverload<const QCPRange &>::of(&QCPAxis::rangeChanged),
            this,&PlotDecorator::onYRangeChanged);


    connect(plot, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(mouseMoveSignal(QMouseEvent*)));
    connect(plot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePressSignal(QMouseEvent*)));
    connect(plot, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(mouseReleaseSignal(QMouseEvent*)));
    connect(plot, SIGNAL(beforeReplot()), this, SLOT(beforeReplot()));

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
        init(graphs.at(0));

    for (const Graph &graph : graphs) {
        minX = qMin(minX,graph.getMinX());
        maxX = qMax(maxX,graph.getMaxX());

        minY = qMin(minY,graph.getMinY());
        maxY = qMax(maxY,graph.getMaxY());
    }

    if (!wasInitiated)
    {
        wasInitiated = true;
        plot->xAxis->setRange(minX,maxX);
        plot->yAxis->setRange(minY,maxY);
    }
}

void PlotDecorator::init(const Graph &graph)
{
    minX = graph.getMinX();
    maxX = graph.getMaxX();

    minY = graph.getMinY();
    maxY = graph.getMaxY();
}

void PlotDecorator::mouseMoveSignal(QMouseEvent *event)
{
    if (draggingLegend)
    {
        QRectF rect = plot->axisRect()->insetLayout()->insetRect(0);
        // since insetRect is in axisRect coordinates (0..1), we transform the mouse position:

        QPointF mousePoint((event->pos().x()- plot->axisRect()->left())/(double)plot->axisRect()->width(),
                           (event->pos().y()- plot->axisRect()->top())/(double)plot->axisRect()->height());
        rect.moveTopLeft(mousePoint-dragLegendOrigin);
        plot->axisRect()->insetLayout()->setInsetRect(0, rect);
        plot->replot();
    }

}

void PlotDecorator::mousePressSignal(QMouseEvent *event)
{

    if (plot->legend->selectTest(event->pos(), false) > 0)
    {
        plot->yAxis->axisRect()->setRangeDrag(0);
        draggingLegend = true;
        // since insetRect is in axisRect coordinates (0..1), we transform the mouse position:
        QPointF mousePoint((event->pos().x() - plot->axisRect()->left())/(double) plot->axisRect()->width(),
                           (event->pos().y() - plot->axisRect()->top())/(double)plot->axisRect()->height());
        dragLegendOrigin = mousePoint- plot->axisRect()->insetLayout()->insetRect(0).topLeft();
    }
}

void PlotDecorator::mouseReleaseSignal(QMouseEvent *event)
{
    Q_UNUSED(event)

    plot->yAxis->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);

    draggingLegend = false;
}

void PlotDecorator::beforeReplot()
{
    // this is to prevent the legend from stretching if the plot is stretched.
    // Since we've set the inset placement to be ipFree, the width/height of the legend
    // is also defined in axisRect coordinates (0..1) and thus would stretch.
    // This is due to change in a future release (probably QCP 2.0) since it's basically a design mistake.
    plot->legend->setMaximumSize(plot->legend->minimumOuterSizeHint());
}


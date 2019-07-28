#include "plotdecorator.h"

PlotDecorator::PlotDecorator(QCustomPlot *plot, QFont legendFont, QObject *parent)
    : QObject(parent), wasInitiated(false), plot(plot),
      legendFont(legendFont), draggingLegend(false)
{
    connect(plot->xAxis, QOverload<const QCPRange &>::of(&QCPAxis::rangeChanged),
            this, &PlotDecorator::onXRangeChanged);
    connect(plot->yAxis, QOverload<const QCPRange &>::of(&QCPAxis::rangeChanged),
            this,&PlotDecorator::onYRangeChanged);


    connect(plot, &QCustomPlot::mouseMove, this, &PlotDecorator::mouseMoveSignal);
    connect(plot, &QCustomPlot::mousePress, this, &PlotDecorator::mousePressSignal);
    connect(plot, &QCustomPlot::mouseRelease, this, &PlotDecorator::mouseReleaseSignal);
    connect(plot, &QCustomPlot::beforeReplot, this, &PlotDecorator::beforeReplot);

    connect(plot, &QCustomPlot::selectionChangedByUser, this, &PlotDecorator::selectionChanged);

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

void PlotDecorator::initPlot(const QList<Graph> &graphs)
{
    plot->clearGraphs();
    for (int i = 0; i < graphs.count(); i++)
        addGraph(graphs.at(i), QCPGraph::lsLine);

    plot->xAxis->setRange(0,1);
    plot->yAxis->setRange(0,1);
    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom |
                          QCP::iSelectPlottables | QCP::iSelectLegend | QCP::iMultiSelect);

    intLegend();

    plot->replot();
}

void PlotDecorator::hideSelectedGraphs()
{
    for (int i=0; i < plot->graphCount(); ++i)
    {
        QCPGraph *graph = plot->graph(i);
        QCPPlottableLegendItem *item = plot->legend->itemWithPlottable(graph);
        if ( graph->selected())
        {
            item->setVisible(false);
            graph->setVisible(false);
        }
    }

    plot->replot();
}

void PlotDecorator::showAllGraphs()
{
    for (int i=0; i < plot->graphCount(); ++i)
    {
        QCPGraph *graph = plot->graph(i);
        QCPPlottableLegendItem *item = plot->legend->itemWithPlottable(graph);

        item->setVisible(true);
        graph->setVisible(true);
    }

    plot->replot();
}

void PlotDecorator::intLegend()
{
    plot->legend->setFont(legendFont);
    plot->legend->setBrush(QBrush(QColor(255,255,255,230)));

    plot->legend->setVisible(true);
    plot->axisRect()->insetLayout()->setInsetPlacement(0, QCPLayoutInset::ipFree);
    plot->axisRect()->insetLayout()->setInsetRect(0, QRectF(0.8, 0, 1, 1));
}


void PlotDecorator::drawPortion(const QList<Graph> &graphs)
{
    qApp->processEvents();
    for (int i = 0; i < graphs.count(); i++)
    {
        const Graph graph = graphs.at(i);
        int index = getGraphIndex(graph.getName());
        if (index != -1)
            plot->graph(index)->addData(graph.getX(),graph.getY());
        else
            qDebug() << "graph with name {" << graph.getName() << "} "
                     << " not found";
    }
    updateRange(graphs);
    plot->replot();
    qApp->processEvents();
}

void PlotDecorator::addGraph(const Graph &graph, QCPGraph::LineStyle lineStyle)
{
    Graph tmp(graph);

    int count = 1;
    while (getGraphIndex(tmp.getName()) != -1)
    {
        QString name = QString(graph.getName() + " (%1)")
                        .arg(count++);
        tmp.setName(name);
    }

    if (graph.getName() != tmp.getName())
        emit addedGraphChangedName(graph.getName(), tmp.getName());

    QCPGraph *newGraph = plot->addGraph();
    newGraph->setPen(graph.getColor());
    newGraph->setLineStyle(lineStyle);
    newGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
    newGraph->setName(tmp.getName());
}

int PlotDecorator::getGraphIndex(const QString &name) const
{
    for (int i = 0; i < plot->graphCount(); i++)
    {
        if (plot->graph(i)->name() == name)
            return i;
    }
    return -1;
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
    if (!draggingLegend)
        return;

    QRectF rect = plot->axisRect()->insetLayout()->insetRect(0);

    double width = static_cast<double>(plot->axisRect()->width());
    double height = static_cast<double>(plot->axisRect()->height());

    QPointF mousePoint((event->pos().x() - plot->axisRect()->left())/width,
                       (event->pos().y() - plot->axisRect()->top())/height);
    rect.moveTopLeft(mousePoint-dragLegendOrigin);
    plot->axisRect()->insetLayout()->setInsetRect(0, rect);
    plot->replot();

}

void PlotDecorator::mousePressSignal(QMouseEvent *event)
{

    if (plot->legend->selectTest(event->pos(), false) <= 0)
        return;

    plot->yAxis->axisRect()->setRangeDrag(0);
    draggingLegend = true;

    double width = static_cast<double>(plot->axisRect()->width());
    double height = static_cast<double>(plot->axisRect()->height());

    QPointF mousePoint((event->pos().x() - plot->axisRect()->left()) / width,
                       (event->pos().y() - plot->axisRect()->top()) / height);
    dragLegendOrigin = mousePoint- plot->axisRect()->insetLayout()->insetRect(0).topLeft();

}

void PlotDecorator::mouseReleaseSignal(QMouseEvent *event)
{
    Q_UNUSED(event)

    plot->yAxis->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);

    draggingLegend = false;
}

void PlotDecorator::beforeReplot()
{
    plot->legend->setMaximumSize(plot->legend->minimumOuterSizeHint());
}

void PlotDecorator::selectionChanged()
{
    for (int i=0; i < plot->graphCount(); ++i)
    {
        QCPGraph *graph = plot->graph(i);
        QCPPlottableLegendItem *item = plot->legend->itemWithPlottable(graph);
        if (item->selected() || graph->selected())
        {
            item->setSelected(true);
            graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
        }
    }
    emit selectionChangedByUser();
}

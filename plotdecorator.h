#ifndef PLOTDECORATOR_H
#define PLOTDECORATOR_H

#include "graph.h"
#include "qcustomplot.h"

#include <QObject>

class PlotDecorator : public QObject
{
    Q_OBJECT
public:
    explicit PlotDecorator(QCustomPlot *plot, QFont legendFont, QObject *parent = nullptr);

signals:
    void selectionChangedByUser();

public slots:
    void drawPortion(const QList<Graph> &graphs);
    void addGraph(const Graph &graph, QCPGraph::LineStyle lineStyle);
    void initPlot(const QList<Graph> &graphs);
    void hideSelectedGraphs();
    void showAllGraphs();

private slots:
    void onXRangeChanged(const QCPRange &range);
    void onYRangeChanged(const QCPRange &range);

    void mouseMoveSignal(QMouseEvent *event);
    void mousePressSignal(QMouseEvent *event);
    void mouseReleaseSignal(QMouseEvent *event);
    void beforeReplot();
    void selectionChanged();

private:

    void updateRange(const QList<Graph> &graphs);
    void init(const Graph &graph);
    void intLegend();

    bool wasInitiated;
    QCustomPlot *plot;

    QFont legendFont;
    bool draggingLegend;
    QPointF dragLegendOrigin;

    int getGraphIndex(const QString &name) const;

    double minX;
    double maxX;

    double minY;
    double maxY;
};

#endif // PLOTDECORATOR_H

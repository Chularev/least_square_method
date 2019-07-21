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

    void intLegend();

signals:

public slots:
    void drawPortion(const QList<Graph> &graphs);

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


    bool wasInitiated;
    QCustomPlot *plot;

    QFont legendFont;
    bool draggingLegend;
    QPointF dragLegendOrigin;

    double minX;
    double maxX;

    double minY;
    double maxY;
};

#endif // PLOTDECORATOR_H

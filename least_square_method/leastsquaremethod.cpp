#include "leastsquaremethod.h"

static const int PORTION_SIZE = 1000;
LeastSquareMethod::LeastSquareMethod(QObject *parent) : QObject(parent),
    graphResult("")
{

}

void LeastSquareMethod::doWork(int windowSize, int shift, QCPGraph *graph)
{
    graphResult = Graph(graph->name() +"_result");
    emit statusChanged(Status::APPROXIMATION_START);
    emit createGraph(graphResult, QCPGraph::lsLine);


    graph->data()->sort();
    for (int i = 0; i < graph->data()->size() && shift > 0; i += shift)
    {
        /*
         * Because the last portion not approximated
         */
        while (i  + windowSize > graph->data()->size() - 1)
        {
            windowSize = windowSize / 2;
            if (windowSize < shift)
                shift = windowSize - 1;
        }

        calculateWindow(i, windowSize, graph);
    }


    // For beauty
    int index = graph->data()->size() - 1;
    xResult.append(graph->data()->at(index)->key);
    yResult.append(graph->data()->at(index)->value);
    doEmit();

    emit statusChanged(Status::APPROXIMATION_FINISH);
}

void LeastSquareMethod::resultGraphChangedName(const QString &oldName, const QString &newName)
{
    Q_UNUSED(oldName);
    graphResult.setName(newName);
}

void LeastSquareMethod::calculateWindow(int start, int windowSize, QCPGraph *graph)
{
    QMap<QString,double> summ;
    summ["n"]   = windowSize;
    summ["x"]   = 0;
    summ["y"]   = 0;
    summ["X^2"] = 0;
    summ["xy"]  = 0;

    int i = 0;
    for (; i < windowSize; i++)
    {
        int index = start + i;
        if (index >= graph->data()->size())
            break;

        double x = graph->data()->at(index)->key;
        double y = graph->data()->at(index)->value;
        summ["x"] += x;
        summ["y"] += y;

        summ["x^2"] += x*x;
        summ["xy"] +=  x*y;
    }

    if (i > 0)
    {
        int index = start + i / 2;
        double xMid = graph->data()->at(index)->key;

        kramer(summ, xMid);
    }

}

static double determinant(double matrix[4])
{
    return matrix[0] * matrix[3] - matrix[1] * matrix[2];
}

void LeastSquareMethod::kramer(const QMap<QString, double> &summ, double xMid)
{

    double matrixD[4] = {summ["x^2"], summ["x"],
                        summ["x"], summ["n"]};
    double D = determinant(matrixD);
    if (D == 0.0)
        return;


    double matrixA[4]= {summ["xy"], summ["x"],
                       summ["y"], summ["n"]};
    double DA =  determinant(matrixA);

    double A = DA / D;
    double B = (summ["y"] - A*summ["x"]) / summ["n"];


    double y = A * xMid + B;

    xResult.append(xMid);
    yResult.append(y);
    if (xResult.size() == PORTION_SIZE)
        doEmit();
}

void LeastSquareMethod::doEmit()
{
    graphResult.setX(xResult.toVector());
    graphResult.setY(yResult.toVector());

    QList<Graph> graphs;
    graphs.append(graphResult);

    emit portionApproximated(graphs) ;

    xResult.clear();
    yResult.clear();
}

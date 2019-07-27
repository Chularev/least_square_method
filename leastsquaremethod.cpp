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
    for (int i = 0; i < graph->data()->size(); i += shift)
    {
        if (i  + windowSize >= graph->data()->size())
            windowSize = shift + 1;

        calculateWindow(i, windowSize, graph);
    }

    if (xResult.size() > 0 && xResult.size() < PORTION_SIZE)
        doEmit();

    emit statusChanged(Status::APPROXIMATION_FINISH);
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
        double x2 = graph->data()->at(index)->key;

        kramer(summ, x2);
    }

}

static double determinant(double matrix[4])
{
    return matrix[0] * matrix[3] - matrix[1] * matrix[2];
}

void LeastSquareMethod::kramer(const QMap<QString, double> &summ, double xMid)
{
    /*
    double matrixD[4] = {summ["x^2"], summ["x"],
                        summ["x"], summ["n"]};
    double D = determinant(matrixD);
    if (D == 0.0)
        return;

    double matrixA[4]= {summ["xy"], summ["x"],
                       summ["y"], summ["n"]};
    double DA =  determinant(matrixA);

    double matrixB[4] = {summ["x^2"], summ["xy"],
                        summ["x"], summ["y"]};
    double DB =  determinant(matrixB);

    double A = DA / D;
    double B = DB / D;


    double y = A * xMid + B;
    */
    double tmp = (summ["n"]*summ["x^2"] - summ["x"]*summ["x"]);
 //   qDebug() << "D = " << tmp;
    if (tmp == 0.0)
        return;

    double A = (summ["n"] * summ["xy"] - (summ["x"]*summ["y"])) / tmp;
    double B = (summ["y"] - A * summ["x"]) / summ["n"];

    xResult.append(xMid);
    yResult.append(A * xMid + B);
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

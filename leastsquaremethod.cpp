#include "leastsquaremethod.h"

static const int PORTION_SIZE = 1000;
LeastSquareMethod::LeastSquareMethod(QObject *parent) : QObject(parent),
    graphResult("")
{

}

void LeastSquareMethod::doWork(int windowSize, int step, QCPGraph *graph)
{
    graphResult = Graph(graph->name() +"_result");
    emit statusChanged(Status::APPROXIMATION_START);
    emit createGraph(graphResult, QCPGraph::lsLine);

    QMap<QString,double> summ;

    graph->data()->sort();
    for (int i = 0, shift = 0; i < graph->data()->size(); i++)
    {
        double x = graph->data()->at(i)->key;
        double y = graph->data()->at(i)->value;
        summ["x"] = summ.contains("x") ? summ["x"] + x : x;
        summ["y"] = summ.contains("y") ? summ["y"] + y : y;

        summ["x^2"] = summ.contains("x^2") ? summ["x^2"] + x*x : x*x;
        summ["xy"] = summ.contains("xy") ? summ["xy"] + x*y : x*y;
        if (i % windowSize == 0 && i > 0)
        {
            shift += step;
            i = shift;
            summ["n"] = windowSize;
            double x1 = graph->data()->at(i - windowSize/2)->key;
            kramer(summ, x1);
            summ.clear();
        }
        if (i == graph->data()->size() && !(i % windowSize == 0 && i > 0))
        {
            summ["n"] = windowSize;
            double x1 = graph->data()->at(i - windowSize/2)->key;
            kramer(summ, x1);
            summ.clear();
        }
    }

    if (xResult.size() > 0 && xResult.size() < PORTION_SIZE)
        doEmit();

    emit statusChanged(Status::APPROXIMATION_FINISH);
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

    double matrixA[4]= {summ["xy"], summ["y"],
                       summ["x"], summ["n"]};
    double DA =  determinant(matrixA);

    double matrixB[4] = {summ["x^2"], summ["x"],
                        summ["xy"], summ["y"]};
    double DB =  determinant(matrixB);

    double A = DA / D;
    double B = DB / D;

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

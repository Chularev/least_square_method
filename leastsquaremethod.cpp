#include "leastsquaremethod.h"

LeastSquareMethod::LeastSquareMethod(QObject *parent) : QObject(parent),
    graph(nullptr)
{

}

void LeastSquareMethod::doWork(int windowSize, int step, QCPGraph *graph)
{
    this->graph = graph;
    QMap<QString,double> summ;

    for (int i = 0, shift = 0; i < graph->data()->size(); i++)
    {
       double x = graph->data()->at(i)->key;
       double y = graph->data()->at(i)->value;
       summ["x"] = summ.contains("x") ? summ["x"] + x : x;
       summ["y"] = summ.contains("y") ? summ["y"] + y : y;

       summ["x^2"] = summ.contains("x^2") ? summ["x^2"] + x*x : x*x;
       summ["xy"] = summ.contains("xy") ? summ["xy"] + x*y : x*y;
       if (i % windowSize == 0)
       {
           shift += step;
           i = shift;
           summ["n"] = windowSize;
           kramer(summ);
           summ.clear();
       }
    }
}
static double determinant(double matrix[4])
{
    return matrix[0] * matrix[2] - matrix[1] * matrix[3];
}

void LeastSquareMethod::kramer(const QMap<QString, double> &summ)
{
    double matrixD[] = {summ["x^2"], summ["x"],
                       summ["x"], summ["n"]};
    double D = determinant(matrixD);
    if (qAbs(0 - D) < 0.000000001)
        return;

    double matrixA[]= {summ["xy"], summ["y"],
                       summ["x"], summ["n"]};
    double DA =  determinant(matrixA);

    double matrixB[] = {summ["x^2"], summ["x"],
                       summ["xy"], summ["y"]};
    double DB =  determinant(matrixB);

    double A = DA / D;
    double B = DB / D;


}

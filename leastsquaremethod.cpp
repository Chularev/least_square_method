#include "leastsquaremethod.h"

LeastSquareMethod::LeastSquareMethod(QObject *parent) : QObject(parent),
    graph(nullptr)
{

}

void LeastSquareMethod::doWork(int windowSize, int shift, QCPGraph *graph)
{
    this->graph = graph;
    for (int i = 0, j = 0; i < graph->data()->size(); i++)
    {
      qDebug()  << graph->data()->at(i)->key;
      qDebug()  << graph->data()->at(i)->value;
    }
}

#include "graph.h"

QList<QColor> Graph::colors;

Graph::Graph(const QString &name)
    : name(name)
{
    color = generateColor(); 
}

Graph::~Graph()
{
    for (int i = 0; i< Graph::colors.count(); i++)
        if (Graph::colors.at(i) == color)
            Graph::colors.removeAt(i);
}

QColor Graph::getColor() const
{
    return color;
}

QColor Graph::generateColor()
{
    QColor color;
    do
    {
        color = QColor(qrand() % 255, qrand() % 255, qrand() % 255);
    } while (Graph::colors.contains(color));
    Graph::colors.append(color);
    return color;
}

QVector<double> Graph::getY() const
{
    return y;
}

void Graph::setY(const QVector<double> &value)
{
    y = value;
}

QVector<double> Graph::getX() const
{
    return x;
}

void Graph::setX(const QVector<double> &value)
{
    x = value;
}

QString Graph::getName() const
{
    return name;
}


double Graph::getMinX() const
{
    return *std::min_element(x.constBegin(), x.constEnd());
}

double Graph::getMinY() const
{
  return *std::min_element(y.constBegin(), y.constEnd());
}

double Graph::getMaxX() const
{
    return *std::max_element(x.constBegin(), x.constEnd());
}

double Graph::getMaxY() const
{
   return *std::max_element(y.constBegin(), y.constEnd());
}

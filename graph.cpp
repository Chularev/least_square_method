#include "graph.h"

QList<QColor> Graph::colors;

Graph::Graph(const QString &name)
    : name(name)
{
    color = generateColor();
    Graph::colors.append(color);
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
    return color;
}

QString Graph::getName() const
{
    return name;
}

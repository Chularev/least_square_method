#ifndef GRAPH_H
#define GRAPH_H

#include <QColor>



class Graph
{
public:
    Graph(const QString &name);
    virtual ~Graph();

    QColor getColor() const;

    QString getName() const;

private:
    QColor generateColor();

    QString name;
    QColor color;
    static QList<QColor> colors;
};

#endif // GRAPH_H

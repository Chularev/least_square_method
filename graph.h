#ifndef GRAPH_H
#define GRAPH_H

#include <QColor>
#include <QVector>



class Graph
{
public:
    Graph(const QString &name);
    virtual ~Graph();

    QColor getColor() const;

    QString getName() const;

    QVector<double> getX() const;
    void setX(const QVector<double> &value);

private:
    QColor generateColor();

    QString name;
    QColor color;
    static QList<QColor> colors;

    QVector<double> y;
    QVector<double> x;
};

#endif // GRAPH_H

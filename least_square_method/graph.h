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

    QVector<double> getY() const;
    void setY(const QVector<double> &value);

    double getMinX() const;
    double getMinY() const;

    double getMaxX() const;
    double getMaxY() const;

    void setName(const QString &value);

private:
    QColor generateColor();

    QString name;
    QColor color;
    static QList<QColor> colors;

    QVector<double> y;
    QVector<double> x;
};

#endif // GRAPH_H

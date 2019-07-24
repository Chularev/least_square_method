#ifndef CSVREADER_H
#define CSVREADER_H

#include "status.h"
#include "graph.h"

#include <QObject>
#include <QTextStream>

class CSVLoader : public QObject
{
    Q_OBJECT
public:
    explicit CSVLoader();

public slots:
    void loadData(const QString &path, const QChar &separator);

signals:
    void statusChanged(Status status, QString message = "") const;
    void initPlot(const QList<Graph> &graphs) const;
    void portionLoaded(const QList<Graph> &graphs) const;

private:
    void parseHeader();
    bool init(const QChar &separator);
    void loadPortion(const QChar &separator);
    void doPortionEmit(const QVector<QList<double>> &xVector, const QVector<QList<double>> &yVector) const;
    bool strToDouble(const QString &str, double *result) const;

    QStringList header;
    QTextStream input;
};

#endif // CSVREADER_H

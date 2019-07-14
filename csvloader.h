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
    void statusChanged(Status status, QString message = "");
    void initPlot(const QList<Graph> &graphs);
    void portionLoaded(const QList<Graph> &graphs);

private:
    void parseHeader(const QStringList &header);
    bool init(const QChar &separator);
    void loadPortion(const QChar &separator);
    void doPortionEmit(const QList<double> &y, const QVector<QList<double>> &xVector);

    int headerSize;
    QTextStream input;
};

#endif // CSVREADER_H

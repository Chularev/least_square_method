#ifndef CSVREADER_H
#define CSVREADER_H

#include "status.h"
#include "graph.h"

#include <QObject>

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

private:
    void parseHeader(const QStringList &header);

    QString path;
    QChar separator;
};

#endif // CSVREADER_H

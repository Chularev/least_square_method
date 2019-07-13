#include "csvloader.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>

CSVLoader::CSVLoader()
{

}

void CSVLoader::loadData(const QString &path, const QChar &separator)
{
    emit statusChanged(Status::START_LOAD_DATA);

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        emit statusChanged(Status::OPEN_FILE_ERROR, file.errorString());
        return;
    }
    QTextStream input(&file);
    if (!input.atEnd())
    {
        QString line = input.readLine();
        QStringList header = line.split(separator);
        parseHeader(header);
    }


    QList<Graph> portion;
    QStringList row;
    while (!input.atEnd()){
        QString line = input.readLine();
        row = line.split(separator);
        qDebug() << row;
    }
    file.close();

    emit statusChanged(Status::FINISH_LOAD_DATA);
}

void CSVLoader::parseHeader(const QStringList &header)
{
    QList<Graph> graphs;

    for (int i = 1; i < graphs.count(); i++)
        if (i > 0)
            graphs.append(Graph(header.at(i)));

    emit initPlot(graphs);
}

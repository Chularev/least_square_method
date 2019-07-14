#include "csvloader.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>

static const int PORTION_SIZE = 50000;

CSVLoader::CSVLoader()
    : headerSize(0)
{

}

void CSVLoader::loadData(const QString &path, const QChar &separator)
{
    emit statusChanged(Status::START_LOAD_DATA);

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        emit statusChanged(Status::OPEN_FILE_ERROR,
                           file.errorString());
        return;
    }

    input.setDevice(&file);

    if (!init(separator))
    {
        emit statusChanged(Status::OPEN_FILE_ERROR,
                           "File is empty");
        return;
    }

    while (!input.atEnd())
        loadPortion(separator);

    file.close();

    emit statusChanged(Status::FINISH_LOAD_DATA);
}

bool CSVLoader::init(const QChar &separator)
{
    if (input.atEnd())
        return false;

    QString line = input.readLine();
    QStringList header = line.split(separator);
    headerSize = header.size();
    parseHeader(header);

    return  true;
}

void CSVLoader::parseHeader(const QStringList &header)
{
    QList<Graph> graphs;

    for (int i = 1; i < header.count(); i++)
        graphs.append(Graph(header.at(i)));

    emit initPlot(graphs);
}

void CSVLoader::loadPortion(const QChar &separator)
{
    QVector<QList<double>> yVector(headerSize - 1);
    QVector<QList<double>> xVector(headerSize - 1);

    for (int i = 0; i < PORTION_SIZE; i++)
    {
        if (input.atEnd())
            break;

        QString line = input.readLine();
        QStringList row = line.split(separator);
        for (int j = 1; j < row.count(); ++j) {
            xVector[j - 1].append(row[j].toDouble());
            yVector[j - 1].append(row[j].toDouble());
        }
    }

   doPortionEmit(yVector,xVector);
}

void CSVLoader::doPortionEmit(const QVector<QList<double>> &yVector, const QVector<QList<double>> &xVector) const
{
    QList<Graph> graphs;
    for (int i = 0; i < xVector.size(); i++) {
        Graph graph("");
        graph.setX(xVector[i].toVector());
        graph.setY(yVector[i].toVector());
        graphs.append(graph);
    }

    emit portionLoaded(graphs);
}

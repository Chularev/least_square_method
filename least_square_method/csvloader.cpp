#include "csvloader.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>

static const int PORTION_SIZE = 50000;

CSVLoader::CSVLoader()
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

void CSVLoader::graphChangedName(const QString &oldName, const QString &newName)
{
    int findNameCount = 0;
    for (int i = 0; i < header.size(); i++)
    {
        if (header.at(i) == oldName)
            findNameCount++;

        if (findNameCount == 2)
        {
            header[i] = newName;
            break;
        }
    }
}

bool CSVLoader::init(const QChar &separator)
{
    if (input.atEnd())
        return false;

    QString line = input.readLine();
    header = line.split(separator);
    parseHeader();

    return  true;
}

void CSVLoader::parseHeader()
{
    QList<Graph> graphs;

    for (int i = 1; i < header.count(); i++)
        graphs.append(Graph(header.at(i)));

    emit initPlot(graphs);
}

void CSVLoader::loadPortion(const QChar &separator)
{
    QVector<QList<double>> yVector(header.size() - 1);
    QVector<QList<double>> xVector(header.size() - 1);

    for (int i = 0; i < PORTION_SIZE; i++)
    {
        if (input.atEnd())
            break;

        QString line = input.readLine();
        QStringList row = line.split(separator);

        double y;
        if (!strToDouble(row[0],&y))
            continue;

        for (int j = 1; j < row.count(); j++)
        {
            double x;
            if (!strToDouble(row[j],&x))
                continue;
            yVector[j - 1].append(y);
            xVector[j - 1].append(x);
        }
    }

   doPortionEmit(xVector,yVector);
}

bool CSVLoader::strToDouble(const QString &str, double *result) const
{

    bool test;
    *result = str.toDouble(&test);
    return test;
}

void CSVLoader::doPortionEmit(const QVector<QList<double>> &xVector,const QVector<QList<double>> &yVector) const
{
    QList<Graph> graphs;
    for (int i = 0; i < xVector.size(); i++) {
        if (xVector[i].size() == 0)
            continue;
        Graph graph(header.at(i + 1));
        graph.setX(xVector[i].toVector());
        graph.setY(yVector[i].toVector());
        graphs.append(graph);
    }

    emit portionLoaded(graphs);
}

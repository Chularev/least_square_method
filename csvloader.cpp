#include "csvloader.h"

#include <QFile>
#include <QTextStream>

CSVLoader::CSVLoader(const QString &path, const QChar &separator)
    : path(path), separator(separator)
{

}

void CSVLoader::loadData()
{
    QStringList row;
    QFile f1(path);
    f1.open(QIODevice::ReadOnly);
    QTextStream s1(&f1);
    while (!s1.atEnd()){
      QString s=s1.readLine();
      row = s.split(separator);
    }
    f1.close();
}

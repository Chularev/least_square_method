#ifndef CSVREADER_H
#define CSVREADER_H

#include <QObject>

class CSVLoader : public QObject
{
    Q_OBJECT
public:
    explicit CSVLoader();

signals:

public slots:
    void loadData(const QString &path, const QChar &separator);
private:
    QString path;
    QChar separator;

};

#endif // CSVREADER_H

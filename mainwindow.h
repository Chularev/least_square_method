#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "graph.h"
#include "status.h"

#include <QMainWindow>
#include <QThread>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void createActions();
    void createMenus();

    void initWorkerThread();

    void updateRange(const QList<Graph> &graphs);

    Ui::MainWindow *ui;

    QMenu *fileMenu;
    QAction *openAct;
    QAction *saveAct;

    QThread workerThread;

private slots:
    void openCSV();
    void saveCSV();
    void statusChanged(Status status, QString message);
    void initPlot(const QList<Graph> &graphs);
    void drawPortion(const QList<Graph> &graphs);

signals:
    void loadCSV(const QString &filePath, const QChar &separator);
};

#endif // MAINWINDOW_H

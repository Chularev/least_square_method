#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "graph.h"
#include "plotdecorator.h"
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
    void initLeastSquareMethodThread();

    Ui::MainWindow *ui;

    QMenu *fileMenu;
    QMenu *graphsMenu;

    QAction *openAct;
    QAction *saveAct;
    QAction *approximationAct;
    QAction *hideSelectedGraphs;
    QAction *hideUnselectedGraphs;
    QAction *showAllGraphs;
    QAction *showAllCounts;

    QThread workerThread;
    QThread leastSquareMethodThread;

    PlotDecorator *plotDecorator;

    Status currentStatus;
    bool process;

private slots:
    void openCSV();
    void saveCSV();
    void approximation();
    void updateActions();
    void statusChanged(Status status, QString message);

signals:
    void loadCSV(const QString &filePath, const QChar &separator);
    void leastSquareMethod(int windowSize, int shift, QCPGraph *graph);
};

#endif // MAINWINDOW_H

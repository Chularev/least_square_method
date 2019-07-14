#include "csvloader.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createActions();
    createMenus();

    initWorkerThread();

    setWindowTitle(tr("Линейная апроксимация методом наименьших квадратов"));
}

MainWindow::~MainWindow()
{
    workerThread.quit();
    workerThread.wait();

    delete ui;
}

void MainWindow::createActions()
{
    openAct = new QAction(tr("&Открыть..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Открыть CSV файл"));
    connect(openAct, &QAction::triggered, this, &MainWindow::openCSV);

    saveAct = new QAction(tr("&Сохранить"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Сохранить выбранные кривые на диск"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::saveCSV);

    saveAct->setEnabled(false);
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&Файл"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
}

void MainWindow::initWorkerThread()
{
    CSVLoader *csvLoader = new CSVLoader();
    csvLoader->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, csvLoader, &QObject::deleteLater);
    connect(this, &MainWindow::loadCSV, csvLoader, &CSVLoader::loadData);
    connect(csvLoader,&CSVLoader::statusChanged,this, &MainWindow::statusChanged, Qt::BlockingQueuedConnection);
    connect(csvLoader,&CSVLoader::initPlot,this,&MainWindow::initPlot, Qt::BlockingQueuedConnection);
    connect(csvLoader,&CSVLoader::portionLoaded,this,&MainWindow::drawPortion, Qt::BlockingQueuedConnection);

    workerThread.start();
}

void MainWindow::openCSV()
{
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    tr("Открыть файл csv"), desktopPath, tr("Файл csv (*.csv)"));
    if (!filePath.isEmpty())
        emit loadCSV(filePath,',');

}

void MainWindow::saveCSV()
{
}

void MainWindow::statusChanged(Status status, QString message)
{
    switch (status)
    {
    case Status::START_LOAD_DATA:
        ui->myStatusBar->setText(tr("Выполняется загрузка данных"));
        break;
    case Status::FINISH_LOAD_DATA:
        ui->myStatusBar->setText(tr("Загрузка данных завершена"));
        break;
    case Status::OPEN_FILE_ERROR:
        QMessageBox::warning(this, tr("Ошибка при открытие документа"),
                             tr("При открытие документа произошла ошибка.\n")
                             + message, QMessageBox::Ok);
        break;
    }

}
void MainWindow::initPlot(const QList<Graph> &graphs)
{
    ui->plot->clearGraphs();
    for (int i = 0; i < graphs.count(); i++)
    {
        const Graph graph = graphs.at(i);
        ui->plot->addGraph();
        ui->plot->graph(i)->setPen(graph.getColor());
        ui->plot->graph(i)->setLineStyle(QCPGraph::lsNone);
        ui->plot->graph(i)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
        ui->plot->graph(i)->setName(graph.getName());
    }
    ui->plot->replot();
}

void MainWindow::drawPortion(const QList<Graph> &graphs)
{
    for (int i = 0; i < graphs.count(); i++)
    {
        const Graph graph = graphs.at(i);
        ui->plot->graph(i)->addData(graph.getX(),graph.getY());
    }
    ui->plot->replot();
}

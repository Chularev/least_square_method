#include "csvloader.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "approximationdialog.h"

#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    currentStatus(Status::START_PROGRAM)
{
    ui->setupUi(this);

    QFont legendFont = font();
    legendFont.setPointSize(9);

    plotDecorator = new PlotDecorator(ui->plot, legendFont, this);


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

    approximationAct = new QAction(tr("&Аппроксимация"), this);
    connect(approximationAct, &QAction::triggered, this, &MainWindow::approximation);
    connect(plotDecorator, &PlotDecorator::selectionChangedByUser, this, &MainWindow::selectionChanged);
    approximationAct->setEnabled(false);
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&Файл"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    menuBar()->addAction(approximationAct);
}

void MainWindow::approximation()
{
    ApproximationDialog dialog(this);
    if (!dialog.exec())
        return;

    if (dialog.isValid())
    {

    }
    else
    {
       statusChanged(Status::ERROR_APPROXIMATION_OPTIONS,"");
    }
}

void MainWindow::selectionChanged()
{
  if (currentStatus != Status::FINISH_LOAD_DATA)
  {
      approximationAct->setEnabled(false);
      return;
  }
  bool test = ui->plot->selectedGraphs().count() == 1;
  approximationAct->setEnabled(test);
}

void MainWindow::initWorkerThread()
{
    CSVLoader *csvLoader = new CSVLoader();
    csvLoader->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, csvLoader, &QObject::deleteLater);
    connect(this, &MainWindow::loadCSV, csvLoader, &CSVLoader::loadData);
    connect(csvLoader,&CSVLoader::statusChanged,this, &MainWindow::statusChanged, Qt::BlockingQueuedConnection);


    connect(csvLoader,&CSVLoader::initPlot, plotDecorator,&PlotDecorator::initPlot, Qt::BlockingQueuedConnection);
    connect(csvLoader,&CSVLoader::portionLoaded,plotDecorator,&PlotDecorator::drawPortion, Qt::BlockingQueuedConnection);

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
    currentStatus = status;
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
    case Status::ERROR_APPROXIMATION_OPTIONS:
        QMessageBox::warning(this, tr("Ошибка при введении размеров скользящего окна"),
                             tr("Количество отсчетов должно быть больше или равно сдвигу.\n")
                             + message, QMessageBox::Ok);
        break;
    case Status::START_PROGRAM:
        break;
    }

}

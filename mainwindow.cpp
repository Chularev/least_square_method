#include "csvloader.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "approximationdialog.h"
#include "leastsquaremethod.h"

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
    initLeastSquareMethodThread();

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
    approximationAct->setEnabled(false);

    hideSelectedGraphs = new QAction(tr("&Скрыть выбранные графики"), this);
    connect(hideSelectedGraphs, &QAction::triggered, plotDecorator, &PlotDecorator::hideSelectedGraphs);
    connect(hideSelectedGraphs, &QAction::triggered,this,[=](){ hideSelectedGraphs->setEnabled(false); });
    hideSelectedGraphs->setEnabled(false);

    showAllGraphs = new QAction(tr("&Показать все графики"), this);
    connect(showAllGraphs, &QAction::triggered, plotDecorator, &PlotDecorator::showAllGraphs);
    connect(hideSelectedGraphs, &QAction::triggered,this,[=](){ showAllGraphs->setEnabled(true); });
    connect(showAllGraphs, &QAction::triggered,this,[=](){ showAllGraphs->setEnabled(false); });
    showAllGraphs->setEnabled(false);


    connect(plotDecorator, &PlotDecorator::selectionChangedByUser, this, &MainWindow::selectionChanged);
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&Файл"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
 //   menuBar()->addAction(approximationAct);
    menuBar()->addAction(hideSelectedGraphs);
    menuBar()->addAction(showAllGraphs);
}

void MainWindow::approximation()
{
    ApproximationDialog dialog(this);
    if (!dialog.exec())
        return;

    if (dialog.isValid())
        emit leastSquareMethod(dialog.getSize(),
                               dialog.getShift(),
                               ui->plot->selectedGraphs().at(0));
    else
       statusChanged(Status::ERROR_APPROXIMATION_OPTIONS,"");

}

void MainWindow::selectionChanged()
{
  if (currentStatus != Status::FINISH_LOAD_DATA)
  {
      approximationAct->setEnabled(false);
      return;
  }
  int count = ui->plot->selectedGraphs().count();
  approximationAct->setEnabled(count == 1);
  hideSelectedGraphs->setEnabled(count > 0);
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

void MainWindow::initLeastSquareMethodThread()
{
    LeastSquareMethod *werker = new LeastSquareMethod;
    werker->moveToThread(&leastSquareMethodThread);
    connect(&leastSquareMethodThread, &QThread::finished, werker, &QObject::deleteLater);
    connect(this, &MainWindow::leastSquareMethod, werker, &LeastSquareMethod::doWork);
    connect(werker, &LeastSquareMethod::statusChanged, this, &MainWindow::statusChanged, Qt::BlockingQueuedConnection);


    leastSquareMethodThread.start();
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

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createActions();
    createMenus();

    setWindowTitle(tr("Линейная апроксимация методом наименьших квадратов"));
}

MainWindow::~MainWindow()
{
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

void MainWindow::openCSV()
{
}

void MainWindow::saveCSV()
{
}


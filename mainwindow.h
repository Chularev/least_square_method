#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

    Ui::MainWindow *ui;

    QMenu *fileMenu;
    QAction *openAct;
    QAction *saveAct;

    QThread workerThread;

private slots:
    void openCSV();
    void saveCSV();
    void statusChanged(Status status);

signals:
    void loadCSV(const QString &filePath, const QChar &separator);
};

#endif // MAINWINDOW_H

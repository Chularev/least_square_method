#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

    Ui::MainWindow *ui;

    QMenu *fileMenu;
    QAction *openAct;
    QAction *saveAct;

private slots:
    void openCSV();
    void saveCSV();
};

#endif // MAINWINDOW_H

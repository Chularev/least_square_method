#ifndef APPROXIMATIONDIALOG_H
#define APPROXIMATIONDIALOG_H

#include <QDialog>

namespace Ui {
class ApproximationDialog;
}

class ApproximationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ApproximationDialog(QWidget *parent = nullptr);
    ~ApproximationDialog();

    bool isValid() const;
    int getShift() const;
    int getSize() const;

private:
    Ui::ApproximationDialog *ui;
};

#endif // APPROXIMATIONDIALOG_H

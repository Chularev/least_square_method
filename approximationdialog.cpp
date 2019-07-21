#include "approximationdialog.h"
#include "ui_approximationdialog.h"

#include <QValidator>

ApproximationDialog::ApproximationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ApproximationDialog)
{
    ui->setupUi(this);

    QIntValidator *validator = new QIntValidator(this);
    ui->sizeEdit->setValidator(validator);
    ui->shiftEdit->setValidator(validator);
}

ApproximationDialog::~ApproximationDialog()
{
    delete ui;
}

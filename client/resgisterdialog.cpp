#include "resgisterdialog.h"
#include "ui_resgisterdialog.h"

resgisterDialog::resgisterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::resgisterDialog)
{
    ui->setupUi(this);
}

resgisterDialog::~resgisterDialog()
{
    delete ui;
}

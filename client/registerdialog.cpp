#include "registerdialog.h"
#include "ui_registerdialog.h"
QString registerDialog::password = "", registerDialog::username = "";
registerDialog::registerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::registerDialog)
{

    ui->setupUi(this);
}

registerDialog::~registerDialog()
{
    delete ui;
}

void registerDialog::on_buttonBox_accepted()
{
    registerDialog::username = ui->lineEdit->text();
    registerDialog::password = ui->lineEdit_2->text();
}


#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include <main.h>
namespace Ui {
class registerDialog;
}

class registerDialog : public QDialog
{
    Q_OBJECT

public:
    static QString username, password;
    explicit registerDialog(QWidget *parent = nullptr);
    ~registerDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::registerDialog *ui;
};

#endif // REGISTERDIALOG_H

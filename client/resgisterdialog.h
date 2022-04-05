#ifndef RESGISTERDIALOG_H
#define RESGISTERDIALOG_H

#include <QDialog>

namespace Ui {
class resgisterDialog;
}

class resgisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit resgisterDialog(QWidget *parent = nullptr);
    ~resgisterDialog();

private:
    Ui::resgisterDialog *ui;
};

#endif // RESGISTERDIALOG_H

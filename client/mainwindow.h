#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    static int logined;
public slots:
    void loginClicked();
    void ipClicked();
private slots:
    void on_register_2_clicked();

private:
    int reg(QString, QString);
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

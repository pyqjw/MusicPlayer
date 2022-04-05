#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "main.h"
#include <QInputDialog>
#include <string>
#include <cstring>
extern int val;
extern char ip[20];
int MainWindow::logined = 0;
using namespace std;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    logined = 0;
    ui->setupUi(this);
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(loginClicked()));
    connect(ui->actionserver_ip_address, SIGNAL(triggered()), this, SLOT(ipClicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
int lian(string username, string password) {
    int    sockfd, n,rec_len;
    char    recvline[4096], sendline[4096];
    char    buf[MAXLINE];

    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("create socket error\n");
        exit(0);
    }

    if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }


    int x;
    Node tmp;
    for(int i = 0; i < 16; i++) {
        if(i < username.length()) {
            tmp.s1[i] = username[i];
        } else tmp.s1[i] = 0;
        if(i < password.length()) {
            tmp.s2[i] = password[i];
        } else tmp.s2[i] = 0;
    }
    tmp.sign = 1;
    *((Node*)sendline) = tmp;
    cout << tmp.s1 << endl;
    cout << tmp.s2 << endl;
    int num = LOGINFUC;
    send(sockfd, (char*)&num, sizeof(int), 0);

    if((x= send(sockfd, sendline, sizeof(Node), 0)) < 0)
    {
    printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
    exit(0);
    }
    cout << "sent" << x << endl;
    if((rec_len = recv(sockfd, buf, MAXLINE,0)) == -1) {
       perror("recv error");
       exit(1);
    }
    int flag = *(int*)(buf);
    buf[rec_len]  = '\0';
    printf("Received : %s ",buf);
    close(sockfd);
    return flag;
}
void MainWindow::loginClicked()
{
    QString str = ui->lineEdit->text();
    QString str2 = ui->lineEdit_2->text();

    int ret = lian(str.toStdString(), str2.toStdString());
    if(ret) {
        logined = ret;
        this->close();
    } else ui->pushButton->setText("login failed!");
}
void MainWindow::ipClicked()
{
    bool ok;

    QString text = QInputDialog::getText(this, tr("server ip"),tr("server ip address"), QLineEdit::Normal,0, &ok);
    std::strcpy(ip, text.toStdString().c_str());
}

int MainWindow::reg(QString username, QString password) {
    int    sockfd, n,rec_len;
    char    recvline[4096], sendline[4096];
    char    buf[MAXLINE];

    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("create socket error\n");
        exit(0);
    }

    if( ::connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }
    *(int*)sendline = REGFUC;
    send(sockfd, sendline, sizeof(int), 0);
    Node node;
    strcpy(node.s1, username.toStdString().c_str());
    strcpy(node.s2, password.toStdString().c_str());
    *(Node*)sendline = node;
    send(sockfd, sendline, sizeof(Node), 0);
    recv(sockfd, recvline, sizeof(int), 0);
    return *(int*)recvline;
}
void MainWindow::on_register_2_clicked()
{
    QString username, password;
    registerDialog *rg = new registerDialog(this);
    rg->setWindowModality(Qt::ApplicationModal);
    rg->show();
    rg->exec();
    int ret = reg(registerDialog::username, registerDialog::password);
    if(ret == 0) {
        QMessageBox::information(this, "","register failed");
    } else {
        QMessageBox::information(this, "", "register successful");
    }
};



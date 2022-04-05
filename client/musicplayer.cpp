#include "musicplayer.h"
#include "ui_musicplayer.h"
#include<QHBoxLayout>
#include<QLabel>
#include <thread>
#include <QTimer>
using namespace std;
extern MusicFile musicfile[200];
extern int filecount;
void go(MusicPlayer * mp) {
    char buff[2048];
    while(true) {
        if(mp->qp->canReadLine()) {
            int ll = mp->qp->readLine(buff, 2048);
            if(ll < 6)continue;
            qDebug() << buff;
            if(buff[4] == 'L' && buff[5] == 'E') {
                double len;
                sscanf(buff+11, "%lf", &len);
                mp->ui->label_2->setText(QString::number(len));
            } else if(buff[4] == 'T' && buff[5] == 'I') {
                double len;
                sscanf(buff, "ANS_TIME_POSITION=%lf", &len);
                mp->ui->label->setText(QString::number(len));
            } else if(buff[0] == 'E' && buff[1] == 'x') {
                mp->isPlaying = 0;
                mp->ui->pushButton->setText("Play");
            }
        }
    }
}
MusicPlayer::MusicPlayer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MusicPlayer)
{
    ui->setupUi(this);
    if(MainWindow::logined == 1) {
        ui->uploadButton->setHidden(true);
    }
    QTimer *timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start(400);
    qp = new QProcess();
    std::thread *t1 = new std::thread(go, this);
    t1->detach();

    mkdir("./music", 0777);
    if( inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0){
    exit(0);
    }
    loadMusicList();
    dispMusic();
}

void MusicPlayer::dispMusic() {
    ui->listWidget->clear();

    for(int i = 0; i < filecount; i++) {
        QWidget *wContainer = new QWidget(ui->listWidget);
        QHBoxLayout *hLayout = new QHBoxLayout(wContainer);

        QLabel *placeLabel = new QLabel(tr(musicfile[i].name));
        QLabel *dateLabel = new QLabel((QString::number(musicfile[i].size)));
        QLabel *alarmMsgLabel = new QLabel(tr(musicfile[i].isDownload?"Downloaded":"not Downloaded"));
        //QPushButton *pDeleteBtn = new QPushButton(QIcon(),tr("delete"));
        hLayout->addStretch(1);//将空白没有widget的地方分成了若干份，按比例分配
        hLayout->addWidget(placeLabel);
        hLayout->addStretch(1);
        hLayout->addWidget(dateLabel);
        hLayout->addStretch(1);//将空白没有widget的地方分成了若干份，按比例分配
        hLayout->addWidget(alarmMsgLabel);
        hLayout->setContentsMargins(0,0, 0, 0);//关键代码，如果没有很可能显示不出来
     // wContainer->setLayout(hLayout);//如果layout在创建时就已经将父窗口指针当参数，那就不用setlayout
        wContainer->resize(350,200);
      // wContainer->show();
        QListWidgetItem *alarmItem = new QListWidgetItem(ui->listWidget);
        ui->listWidget->setItemWidget(alarmItem,wContainer);
    }
}

void MusicPlayer::onTimer()
{
    if(isPlaying == 1) {
        qp->write("get_time_length                                                           \n");
        //usleep(350000);
        qp->write("get_time_pos                                                                \n");
    }
}
void MusicPlayer::loadMusicList() {
    int    sockfd, n,rec_len;
    char    recvline[4096], sendline[4096];
    char    buf[MAXLINE];

    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
        exit(0);
    }

    if( ::connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }
    int sign = LOADMUSIC;
    send(sockfd, (char*)&sign, sizeof(int), 0);
    if((rec_len = recv(sockfd, buf, sizeof(int),0)) == -1) {
       perror("recv error");
       exit(1);
    }
    filecount = *(int*)buf;
    rec_len = recv(sockfd, buf, sizeof(MusicFile)*filecount, 0);
    memcpy(musicfile, buf, sizeof(MusicFile)*filecount);
    for(int i = 0; i < filecount; i++) {
        string s = "./music/";
        s += musicfile[i].name;
        if(access(s.c_str(), F_OK) == 0)
            musicfile[i].isDownload = 1;
        else
            musicfile[i].isDownload = 0;
    }
    printf("Received : %s ",buf);
    ::close(sockfd);
}
MusicPlayer::~MusicPlayer()
{
    delete ui;
}

void MusicPlayer::on_pushButton_clicked()
{
    cout << ui->listWidget->currentRow() << endl;
    int row = ui->listWidget->currentRow();
    if(isPlaying) {
        if(isPlaying == 1)ui->pushButton->setText("paused");
        else ui->pushButton->setText("Playing");
        qp->write("pause\n");
        isPlaying = -isPlaying;
    } else {
        isPlaying = 1;
        ui->pushButton->setText("Playing");
        QStringList args;
        args << "-slave";
        args << "-quiet";
        args << "music/"+QString(musicfile[row].name);
        qp->start("mplayer", args);
    }
    //usleep(150000);

    //char buff[1024];
    //isPlaying = true;
    //qp->write("get_time_length\n");
    //qp->kill();
    //qp->write("pause\n");
    //qp->write("get_percent_pos\n");
    //qp->close();
}


void MusicPlayer::on_pushButton_2_clicked()
{
    cout << ui->listWidget->currentRow() << endl;
    int row = ui->listWidget->currentRow();
    if(musicfile[row].isDownload) {
      return ;
    }
    int sockfd;
    char *buff[MAXLINE];
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    ::connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    *(int*)buff = DOWNLOAD;
    send(sockfd, buff, sizeof(int), 0);
    *(int*)buff = row;
    send(sockfd, buff, sizeof(int), 0);
    int size = musicfile[row].size;
    char fname[128] = "./music/";
    strcat(fname, musicfile[row].name);
    FILE *file = fopen(fname, "wb+");
    while(size) {
      int sendsize;
      if(size > 2048)
          sendsize = 2048;
      else sendsize = size;
      recv(sockfd, buff, sendsize, 0);
      fwrite(buff, 1, sendsize, file);
      size -= sendsize;
    }
    fclose(file);
    musicfile[row].isDownload = 1;
    dispMusic();
}


void MusicPlayer::on_listWidget_currentRowChanged(int currentRow)
{
    if(isPlaying)
        qp->write("stop\n");
    isPlaying = 0;
    ui->label_3->setText(musicfile[currentRow].name);
}


void MusicPlayer::closeEvent(QCloseEvent * e)
{
    qp->kill();
}


void MusicPlayer::on_uploadButton_clicked()    //upload
{
    QString fileName = QFileDialog::getOpenFileName(this,
                         tr("文件对话框！"),"/home"
                         ,
                         tr("文件(*);;"
                          ) );
    struct stat st;
    if(fileName == "") return ;
    stat(fileName.toUtf8(), &st);
    MusicFile mf;
    mf.size = st.st_size;
    char tmp[128];
    strcpy(tmp, fileName.toStdString().c_str());
    int ll = strlen(tmp);
    for(int i = ll-1; i >= 0; i--) {
        if(tmp[i] == '\\' || tmp[i] == '/') {
            strcpy(mf.name, tmp+i+1);
            break;
        }
    }
    int    sockfd, n,rec_len;
    char    recvline[4096], sendline[4096];
    char    buf[MAXLINE];
    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
        exit(0);
    }

    if( ::connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }
    int sign = UPLOAD;
    send(sockfd, (char*)&sign, sizeof(int), 0);
    send(sockfd, (char*)&mf, sizeof(MusicFile), 0);
    FILE *file = fopen(fileName.toStdString().c_str(), "rb+");
    int size = mf.size;
    while(size) {
        int sendsize;
        if(size > 2048)
            sendsize = 2048;
        else sendsize = size;
        fread(sendline, 1, sendsize, file);

        send(sockfd, sendline, sendsize, 0);
        size -= sendsize;
    }
    fclose(file);
    ::close(sockfd);
    loadMusicList();
    dispMusic();
}


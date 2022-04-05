
#include <QApplication>
#include<main.h>

using namespace std;
int var = 0;
char ip[20]= "127.0.0.1";
MusicFile musicfile[200];
int filecount = 0;
struct sockaddr_in  servaddr;
int main(int argc, char** argv)
{
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(12348);
    if( inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0){
        exit(0);
    }
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    a.exec();
    if(MainWindow::logined) {
        MusicPlayer mp;
        mp.show();
        a.exec();
    }
    return 0;
}

int getMusicFile() {

}

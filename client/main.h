#ifndef MAIN_H
#define MAIN_H
#define MAXLINE 4096
#define LOGINFUC 1
#define LOADMUSIC 2
#define DOWNLOAD 3
#define REGFUC 4
#define UPLOAD 5
#include<errno.h>
#include<sqlite3.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<stdio.h>
#include<netinet/in.h>
#include<unistd.h>

#include<arpa/inet.h>
#include <sys/io.h>
#include <dirent.h>
#include <iostream>
#include<stdio.h>
#include <QDebug>
#include<QString>
#include<stdlib.h>
#include<string.h>
#include <string>
#include <QMessageBox>
#include <registerdialog.h>
#include <QProcess>
#include "mainwindow.h"
#include "musicplayer.h"
#include <QFileDialog>
extern int val;
extern char ip[20];
struct Node {
    int sign;
    char s1[16];
    char s2[16];
};
struct MusicFile {
    char name[128];
    int size;
    int isDownload;
};
extern struct sockaddr_in  servaddr;
extern MusicFile musicfile[200];
extern int filecount;
int main(int argc, char** argv);
int getMusicFile();

#endif // MAIN_H

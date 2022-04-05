/* File Name: server.c */
#include <unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<thread>
#include<sqlite3.h>
#include<errno.h>
#include<sys/stat.h>
#include <sys/io.h>
#include <dirent.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <iostream>
#define DEFAULT_PORT 8000
#define LOGINFUC 1
#define LOADMUSIC 2
#define DOWNLOAD 3
#define REGFUC 4
#define UPLOAD 5
#define MAXLINE 4096

using namespace std;
struct MusicFile {
    char name[128];
    int size;
    int isDownload;
};
MusicFile musicfile[200];
int filecount;
struct Node {
    int sign;
    char s1[16];
    char s2[16];
};
sqlite3 *db;
void serve(int);
void downloadmusic(int connect_fd) {
    char buff[MAXLINE];
    char fname[64] = "./music/";
    recv(connect_fd, buff, sizeof(int), 0);
    int index;
    index = *(int*)buff;
    strcat(fname, musicfile[index].name);
    FILE *file = fopen(fname, "rb+");
    int size = musicfile[index].size;
    while(size) {
        int sendsize;
        if(size > 2048)
            sendsize = 2048;
        else sendsize = size;
        fread(buff, 1, sendsize, file);

        send(connect_fd, buff, sendsize, 0);
        size -= sendsize;
    }
    fclose(file);
    close(connect_fd);
}
void readMusic() {
    mkdir("./music", 0777);
    DIR *dir;
    struct stat st;
    struct dirent *ptr;
    dir = opendir("./music");
    filecount = 0;
    while ((ptr=readdir(dir)) != NULL)
    {
        cout << ptr->d_name << endl;
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
            continue;
        string name = "./music/";
        name += ptr->d_name;
        stat(name.c_str(), &st);
        strcpy(musicfile[filecount].name, ptr->d_name);
        musicfile[filecount].size = (int)st.st_size;
        cout << (int)st.st_size << endl;
        filecount++;
    }
}
int main(int argc, char** argv)
{
    readMusic();
    int    socket_fd, connect_fd;
    struct sockaddr_in     servaddr;
    char    buff[4096];
    int     n;
    sqlite3_open("user.db", &db);
    sqlite3_exec(db, "create table user(username char(16) primary key, password char(16), permission(20));", 0, 0, (char**)buff);
    //初始化Socket
    if( (socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
    printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
    exit(0);
    }
    //初始化
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//IP地址设置成INADDR_ANY,让系统自动获取本机的IP地址。
    servaddr.sin_port = htons(12348);//设置的端口为DEFAULT_PORT

    //将本地地址绑定到所创建的套接字上
    if( bind(socket_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
    printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
    exit(0);
    }
    //开始监听是否有客户端连接
    if( listen(socket_fd, 10) == -1){
    printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
    exit(0);
    }
    printf("======waiting for client's request======\n");
    while(1){
    //阻塞直到有客户端连接，不然多浪费CPU资源。
        if( (connect_fd = accept(socket_fd, (struct sockaddr*)NULL, NULL)) == -1){
            printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
            continue;
        }
        //接受客户端传过来的数据
        std::thread t(serve, connect_fd);
        t.detach();
    }
    close(socket_fd);
}

void login(int connect_fd) {
    char buff[4096];
    char *errormsg;
    char **result;
    recv(connect_fd, buff, MAXLINE, 0);
    char *sql;
    cout << "kaishi jieshou" << endl;
    Node tmp = *((Node*)buff);
    cout << tmp.sign << " " << tmp.s1 << " " << tmp.s2 << endl;
    int row, col;
    string Sql, username(tmp.s1), password(tmp.s2);
    Sql = string("select * from user where username =")+"'"+username+"'";
    cout << Sql << endl;
    sqlite3_get_table(db, Sql.c_str(), &result, &row, &col, &errormsg);
    int flag = 0;
    if(row == 1 && string(result[1+col]) == password) {
        if(string(result[2+col]) == "admin") {
            flag = 2;
        }else
            flag = 1;
    }

    send(connect_fd, (char*)&flag, sizeof(int), 0);
    close(connect_fd);
}
void loadmusic(int connect_fd) {
    char buff[4096];
    *(int*)buff = filecount;
    memcpy(buff+sizeof(int), musicfile, filecount*sizeof(MusicFile));
    send(connect_fd, buff, sizeof(int)+filecount*sizeof(MusicFile), 0);
    close(connect_fd);
}
void reg(int connect_fd) {
    char recvline[4096], sendline[4096];
    char *errormsg;
    int row, col;
    char **result;
    recv(connect_fd, recvline, sizeof(Node), 0);
    Node node = *(Node*)recvline;
    string username = node.s1, password = node.s2;
    string Sql = string("select * from user where username =")+"'"+username+"'";
    cout << Sql << endl;
    sqlite3_get_table(db, Sql.c_str(), &result, &row, &col, &errormsg);
    int flag = 0;
    if(row != 0)
        flag = 0;
    else {
        Sql = "insert into user values('"+username+"','" + password + "', 'user');";
            sqlite3_exec(db, Sql.c_str(), 0, 0, &errormsg);
        flag = 1;
    }
    cout << flag << endl;
    send(connect_fd, (char*)&flag, sizeof(int), 0);
    close(connect_fd);
}
void upload(int sockfd) {
    char buff[2048];
    recv(sockfd, buff, sizeof(MusicFile), 0);
    MusicFile mf = *(MusicFile*)buff;
    int size = mf.size;
    char fname[64] = "./music/";
    strcat(fname, mf.name);
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
    readMusic();
}
void serve(int connect_fd) {
    char buff[4096];
    int n = recv(connect_fd, buff, sizeof(int), 0);
    cout << n << endl;
    switch(*(int*)buff) {
    case LOGINFUC:
        login(connect_fd);
        break;
    case LOADMUSIC:
        loadmusic(connect_fd);
        break;
    case DOWNLOAD:
        downloadmusic(connect_fd);
        break;
    case REGFUC:
        reg(connect_fd);
        break;
    case UPLOAD:
        upload(connect_fd);
        break;
    }
}

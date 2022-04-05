#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <QMainWindow>
#include"main.h"
namespace Ui {
class MusicPlayer;
}

class MusicPlayer : public QMainWindow
{
    Q_OBJECT

public:
    explicit MusicPlayer(QWidget *parent = nullptr);
    ~MusicPlayer();
    void loadMusicList();
    void dispMusic();
    int isPlaying = 0;
    QProcess *qp = NULL;
    Ui::MusicPlayer *ui;
private slots:
    void onTimer();
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_listWidget_currentRowChanged(int currentRow);

    void closeEvent(QCloseEvent *e);

    void on_uploadButton_clicked();

private:

};

#endif // MUSICPLAYER_H

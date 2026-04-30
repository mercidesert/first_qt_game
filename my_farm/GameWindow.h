#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMediaPlayer>
#include <QAudioOutput>  // Qt 6 替代 QMediaPlaylist

#include <QWidget>
#include <QTimer>
#include <QRect>
#include "IScene.h"

class GameWindow : public QWidget {
    Q_OBJECT
public:
    GameWindow(QWidget *parent = nullptr);
    ~GameWindow();

    void changeScene(int stateIndex);

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void gameTick();
    void fastTick();

private:
    void drawGlobalHUD(QPainter &painter);
    void drawTextbox(QPainter &painter, int x, int y, int w, int h, QString text);

    int currentState;
    IScene* scenes[7];

    QTimer *timer;
    QTimer *fastTimer;
    QRect exitBtnRect;

    // Qt 6 写法
    QMediaPlayer *bgmPlayer;
    QAudioOutput *audioOutput;  // 替代 playlist
};

#endif

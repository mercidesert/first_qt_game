#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

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
};

#endif
//游戏主窗口类，管理场景切换、刷新计时器、处理用户输入和绘制全局HUD界面

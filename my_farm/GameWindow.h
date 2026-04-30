#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>
#include <QTimer>
#include <QRect>      // 补充 QRect 头文件
#include "IScene.h"

class GameWindow : public QWidget {
    Q_OBJECT
public:
    GameWindow(QWidget *parent = nullptr);
    ~GameWindow();

    // 控制场景切换的接口
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
    IScene* scenes[7]; // 存放7个独立场景的多态指针

    QTimer *timer;
    QTimer *fastTimer;
    QRect exitBtnRect;
};

#endif // GAMEWINDOW_H

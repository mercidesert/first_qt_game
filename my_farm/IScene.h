#ifndef ISCENE_H
#define ISCENE_H
#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>

class GameWindow; // 前向声明方便场景切状态

class IScene {
public:
    GameWindow* window;
    IScene(GameWindow* w) : window(w) {}
    virtual ~IScene() = default;

    virtual void enter() {}
    virtual void gameTick() {}
    virtual void fastTick() {}
    virtual void draw(QPainter &painter) = 0;
    virtual void keyPressEvent(QKeyEvent *event) {}
    virtual void keyReleaseEvent(QKeyEvent *event) {}
    virtual void mousePressEvent(QMouseEvent *event) {}
    virtual void onCancel() {}
};
#endif
//场景接口类，定义了游戏所有状态需要实现的统一方法，通过多态让GameWindow统一管理不同场景

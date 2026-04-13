#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>
#include <QKeyEvent>
#include <QTimer>
#include "farmtile.h"
#include "player.h"

class GameScene : public QGraphicsScene {
    Q_OBJECT // 只要用到信号和槽（比如QTimer），必须加这个宏

public:
    GameScene(QObject *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override; // 监听键盘

private slots:
    void onDayPass(); // 时间流逝槽函数

private:
    Player *player;
    QList<FarmTile*> farmGrid; // 保存所有的土地
    const int TILE_SIZE = 40;  // 每一个格子的大小
    const int GRID_WIDTH = 10; // 农场宽 10 格
    const int GRID_HEIGHT = 10;// 农场高 10 格

    QTimer *dayTimer; // 控制植物生长的计时器
};

#endif // GAMESCENE_H

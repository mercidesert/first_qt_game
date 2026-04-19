#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>
#include <QTimer>
#include <QKeyEvent>
#include <QVector>
#include "farmtile.h"
#include "player.h"

class GameScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit GameScene(QObject *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onDayPass();

private:
    const int TILE_SIZE = 48; // 每个格子的像素大小
    const int GRID_WIDTH = 10;
    const int GRID_HEIGHT = 10;

    Player *player;
    QVector<FarmTile*> farmGrid;
    QTimer *dayTimer;
};

#endif

#include "gamescene.h"

GameScene::GameScene(QObject *parent) : QGraphicsScene(parent) {
    // 1. 初始化地图
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            FarmTile *tile = new FarmTile(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE);
            addItem(tile);
            farmGrid.append(tile);
        }
    }

    // 2. 初始化玩家
    player = new Player(TILE_SIZE);
    addItem(player);
    player->setPos(0, 0);

    // 3. 游戏时间循环
    dayTimer = new QTimer(this);
    connect(dayTimer, &QTimer::timeout, this, &GameScene::onDayPass);
    dayTimer->start(10000); // 每10秒植物长一轮
}

void GameScene::keyPressEvent(QKeyEvent *event) {
    int x = player->x();
    int y = player->y();

    if (event->key() == Qt::Key_W && y > 0) y -= TILE_SIZE;
    else if (event->key() == Qt::Key_S && y < (GRID_HEIGHT-1)*TILE_SIZE) y += TILE_SIZE;
    else if (event->key() == Qt::Key_A && x > 0) x -= TILE_SIZE;
    else if (event->key() == Qt::Key_D && x < (GRID_WIDTH-1)*TILE_SIZE) x += TILE_SIZE;
    else if (event->key() == Qt::Key_Space) {
        int gridX = player->x() / TILE_SIZE;
        int gridY = player->y() / TILE_SIZE;
        farmGrid[gridY * GRID_WIDTH + gridX]->interact();
        return;
    }

    player->setPos(x, y);
}

void GameScene::onDayPass() {
    for (FarmTile *tile : farmGrid) {
        tile->grow();
    }
}

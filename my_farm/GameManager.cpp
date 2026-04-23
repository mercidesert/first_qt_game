#include "GameManager.h"

GameScene::GameScene(QObject *parent) : QGraphicsScene(parent) {
    // 铺设农地
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            FarmTile *tile = new FarmTile(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE);
            addItem(tile);
            farmGrid.append(tile);
        }
    }

    // 添加玩家
    player = new Player(TILE_SIZE);
    addItem(player);
    player->setPos(0, 0);

    dayTimer = new QTimer(this);
    connect(dayTimer, &QTimer::timeout, this, &GameScene::onDayPass);
    dayTimer->start(5000); // 5秒生长一次
}

void GameScene::keyPressEvent(QKeyEvent *event) {
    int x = player->x();
    int y = player->y();

    if (event->key() == Qt::Key_W && y > 0) y -= TILE_SIZE;
    else if (event->key() == Qt::Key_S && y < (GRID_HEIGHT-1)*TILE_SIZE) y += TILE_SIZE;
    else if (event->key() == Qt::Key_A && x > 0) x -= TILE_SIZE;
    else if (event->key() == Qt::Key_D && x < (GRID_WIDTH-1)*TILE_SIZE) x += TILE_SIZE;
    else if (event->key() == Qt::Key_Space) {
        int index = (y / TILE_SIZE) * GRID_WIDTH + (x / TILE_SIZE);
        if (farmGrid[index]->interact()) {
            score += 50;
            emit scoreChanged(score); // 发出信号通知 UI 更新
        }
        return;
    }
    player->setPos(x, y);
}

void GameScene::onDayPass() {
    for (FarmTile *tile : farmGrid) tile->grow();
}

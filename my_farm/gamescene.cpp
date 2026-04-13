#include "gamescene.h"
#include <QDebug>

GameScene::GameScene(QObject *parent) : QGraphicsScene(parent) {
    // 1. 铺设农田网格
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            FarmTile *tile = new FarmTile(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE);
            addItem(tile);
            farmGrid.append(tile); // 存入列表方便后续查找
        }
    }

    // 2. 添加玩家
    player = new Player(TILE_SIZE);
    addItem(player);
    player->setPos(0, 0); // 初始位置

    // 3. 启动时间流逝（模拟星露谷的每天/每tick）
    dayTimer = new QTimer(this);
    connect(dayTimer, &QTimer::timeout, this, &GameScene::onDayPass);
    dayTimer->start(5000); // 为了测试，每 5 秒钟植物生长一次（现实中对应一天结束）
}

void GameScene::keyPressEvent(QKeyEvent *event) {
    int currentX = player->x();
    int currentY = player->y();

    // WASD 移动逻辑 (按格子移动)
    if (event->key() == Qt::Key_W && currentY > 0) {
        player->setPos(currentX, currentY - TILE_SIZE);
    } else if (event->key() == Qt::Key_S && currentY < (GRID_HEIGHT - 1) * TILE_SIZE) {
        player->setPos(currentX, currentY + TILE_SIZE);
    } else if (event->key() == Qt::Key_A && currentX > 0) {
        player->setPos(currentX - TILE_SIZE, currentY);
    } else if (event->key() == Qt::Key_D && currentX < (GRID_WIDTH - 1) * TILE_SIZE) {
        player->setPos(currentX + TILE_SIZE, currentY);
    }
    // 空格键交互逻辑 (种田)
    else if (event->key() == Qt::Key_Space) {
        // 计算玩家现在踩在哪个格子上
        int gridX = player->x() / TILE_SIZE;
        int gridY = player->y() / TILE_SIZE;
        int index = gridY * GRID_WIDTH + gridX;

        if (index >= 0 && index < farmGrid.size()) {
            farmGrid[index]->interact(); // 让脚下这块土地触发动作！
        }
    }
}

void GameScene::onDayPass() {
    qDebug() << "新的一天开始了！植物正在生长...";
    // 遍历所有土地，调用生长逻辑
    for (FarmTile *tile : farmGrid) {
        tile->grow();
    }
}

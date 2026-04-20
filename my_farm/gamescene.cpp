#include "gamescene.h"
#include <QDebug>
#include <QFont>
#include <QGraphicsView> // 【必须加这个头文件，否则 views() 不能正常工作】

GameScene::GameScene(QObject *parent) : QGraphicsScene(parent) {
    // 1. 铺设农田网格
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            FarmTile *tile = new FarmTile(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE);
            addItem(tile);
            farmGrid.append(tile);
        }
    }

    // 2. 添加玩家
    player = new Player(TILE_SIZE);
    addItem(player);
    player->setPos(0, 0);

    // 3. 初始化计分板
    score = 0;
    scoreText = new QGraphicsTextItem();
    scoreText->setPlainText(QString("Money: $ %1").arg(score));
    scoreText->setDefaultTextColor(Qt::yellow);
    scoreText->setFont(QFont("Consolas", 16, QFont::Bold));
    scoreText->setZValue(100);
    scoreText->setPos(20, 20); // 初始位置
    addItem(scoreText);

    // 4. 启动时间流逝
    dayTimer = new QTimer(this);
    connect(dayTimer, &QTimer::timeout, this, &GameScene::onDayPass);
    dayTimer->start(10000);
}

void GameScene::keyPressEvent(QKeyEvent *event) {
    int currentX = player->x();
    int currentY = player->y();

    // --- 1. 处理移动和交互逻辑 ---
    if (event->key() == Qt::Key_W && currentY > 0) {
        player->setPos(currentX, currentY - TILE_SIZE);
    } else if (event->key() == Qt::Key_S && currentY < (GRID_HEIGHT - 1) * TILE_SIZE) {
        player->setPos(currentX, currentY + TILE_SIZE);
    } else if (event->key() == Qt::Key_A && currentX > 0) {
        player->setPos(currentX - TILE_SIZE, currentY);
    } else if (event->key() == Qt::Key_D && currentX < (GRID_WIDTH - 1) * TILE_SIZE) {
        player->setPos(currentX + TILE_SIZE, currentY);
    }
    else if (event->key() == Qt::Key_Space) {
        int gridX = player->x() / TILE_SIZE;
        int gridY = player->y() / TILE_SIZE;
        int index = gridY * GRID_WIDTH + gridX;

        if (index >= 0 && index < farmGrid.size()) {
            if (farmGrid[index]->interact()) {
                score += 50;
                updateScoreDisplay();
                qDebug() << "成功收获！当前金币：" << score;
            }
        }
    }

    // 无论按了什么键（不管是走路还是收割），在函数的最后，强制刷新计分板的位置。
    // 这样即便以后镜头跟着玩家走，计分板也会由于这一行代码被“拉回”到屏幕左上角。
    if (!this->views().isEmpty()) {
        // 获取第一个观察这个场景的视图（即你的游戏窗口）
        QGraphicsView *currentView = this->views().first();

        // 将视图窗口的左上角坐标 (20, 20) 转换成游戏场景内部的坐标
        QPointF topLeftInScene = currentView->mapToScene(20, 20);

        // 把计分板文字移动到那个转换后的位置
        scoreText->setPos(topLeftInScene);
    }
}

void GameScene::updateScoreDisplay() {
    scoreText->setPlainText(QString("Money: $ %1").arg(score));
}

void GameScene::onDayPass() {
    qDebug() << "新的一天开始了！植物正在生长...";
    for (FarmTile *tile : farmGrid) {
        tile->grow();
    }
}

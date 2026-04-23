#include "GameWindow.h"
#include <QPainter>
#include <QRandomGenerator>

GameWindow::GameWindow(QWidget *parent)
    : QWidget(parent), currentSeason(Winter), timeTicks(0), isGameOver(false),
    isHarvesting(false), harvestAnimTimer(0) // 初始化动画状态
{
    setFixedSize(MAP_COLS * TILE_W, MAP_ROWS * TILE_H);
    setWindowTitle("简易星露谷 (16x32素材版)");

    for (int r = 0; r < MAP_ROWS; ++r) {
        for (int c = 0; c < MAP_COLS; ++c) {
            map[r][c].isPlowed = false;
            map[r][c].crop = None;
        }
    }

    auto loadAndScale =[](const QString& path) {
        return QPixmap(path).scaled(TILE_W, TILE_H, Qt::IgnoreAspectRatio, Qt::FastTransformation);
    };

    img_bgWinter = loadAndScale(":/images/bg_winter.png");
    img_bgSpring = loadAndScale(":/images/bg_spring.png");
    img_bgSummer = loadAndScale(":/images/bg_summer.png");
    img_bgFall   = loadAndScale(":/images/bg_fall.png");
    img_soilPlowed = loadAndScale(":/images/soil_plowed.png");

    img_playerUp    = loadAndScale(":/images/player_up.png");
    img_playerDown  = loadAndScale(":/images/player_down.png");
    img_playerLeft  = loadAndScale(":/images/player_left.png");
    img_playerRight = loadAndScale(":/images/player_right.png");

    // 【新增】加载收获图片
    img_playerHarvest = loadAndScale(":/images/player_harvest.png");

    img_strawSeed     = loadAndScale(":/images/straw_1_seed.png");
    img_strawSeedling = loadAndScale(":/images/straw_2_seedling.png");
    img_strawGrown    = loadAndScale(":/images/straw_3_grown.png");
    img_strawFruiting = loadAndScale(":/images/straw_4_fruiting.png");

    img_sunSeed     = loadAndScale(":/images/sun_1_seed.png");
    img_sunSeedling = loadAndScale(":/images/sun_2_seedling.png");
    img_sunGrown    = loadAndScale(":/images/sun_3_grown.png");
    img_sunFruiting = loadAndScale(":/images/sun_4_fruiting.png");

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GameWindow::gameTick);
    timer->start(200);
}

GameWindow::~GameWindow() {}

void GameWindow::gameTick() {
    if (isGameOver) return;
    timeTicks++;
    updateCrops();

    // 【新增】处理人物收获动画的倒计时
    if (isHarvesting) {
        harvestAnimTimer--;
        if (harvestAnimTimer <= 0) {
            isHarvesting = false; // 倒计时结束，恢复普通站姿
        }
    }

    if (timeTicks >= 200) changeSeason();
    update();
}

void GameWindow::updateCrops() {
    for (int r = 0; r < MAP_ROWS; ++r) {
        for (int c = 0; c < MAP_COLS; ++c) {
            Tile &t = map[r][c];
            if (t.crop != None && t.stage != Fruiting) {
                bool canGrow = false;
                if (t.crop == Strawberry && currentSeason == Spring) canGrow = true;
                if (t.crop == Sunflower && (currentSeason == Summer || currentSeason == Fall)) canGrow = true;

                if (canGrow) {
                    t.growthTimer++;
                    if (t.growthTimer >= 20) {
                        t.growthTimer = 0;
                        if (t.stage == Seed) t.stage = Seedling;
                        else if (t.stage == Seedling) t.stage = Grown;
                        else if (t.stage == Grown) t.stage = Fruiting;
                    }
                }
            }
        }
    }
}

void GameWindow::changeSeason() {
    timeTicks = 0;
    currentSeason = static_cast<Season>(currentSeason + 1);
    if (currentSeason == NextWinter) {
        isGameOver = true;
        timer->stop();
        return;
    }
    for (int r = 0; r < MAP_ROWS; ++r) {
        for (int c = 0; c < MAP_COLS; ++c) {
            Tile &t = map[r][c];
            if (t.crop == Strawberry && currentSeason != Spring) t.crop = None;
            if (t.crop == Sunflower && currentSeason == Winter) t.crop = None;
        }
    }
}

void GameWindow::keyPressEvent(QKeyEvent *event) {
    if (isGameOver) return;

    if (event->key() == Qt::Key_W) { player.y = qMax(0, player.y - 1); player.dir = Up; }
    if (event->key() == Qt::Key_S) { player.y = qMin(MAP_ROWS - 1, player.y + 1); player.dir = Down; }
    if (event->key() == Qt::Key_A) { player.x = qMax(0, player.x - 1); player.dir = Left; }
    if (event->key() == Qt::Key_D) { player.x = qMin(MAP_COLS - 1, player.x + 1); player.dir = Right; }

    if (event->key() == Qt::Key_1) player.selectedSeed = Strawberry;
    if (event->key() == Qt::Key_2) player.selectedSeed = Sunflower;

    // 动作键 (J 或者 空格)
    if (event->key() == Qt::Key_J || event->key() == Qt::Key_Space) {
        handleAction();
    }
    update();
}


void GameWindow::handleAction() {
    int targetX = player.x;
    int targetY = player.y;
    switch (player.dir) {
    case Up: targetY--; break;
    case Down: targetY++; break;
    case Left: targetX--; break;
    case Right: targetX++; break;
    }

    if (targetX < 0 || targetX >= MAP_COLS || targetY < 0 || targetY >= MAP_ROWS) return;

    Tile &t = map[targetY][targetX];

    if (!t.isPlowed) {
        // 未开垦的土地，进行开垦并随机给种子
        t.isPlowed = true;
        int rng = QRandomGenerator::global()->bounded(100);
        if (rng < 10) inventory.strawberrySeeds++;
        else if (rng < 35) inventory.sunflowerSeeds++;
    } else {
        if (t.crop != None && t.stage == Fruiting) {
            // 收获逻辑
            isHarvesting = true;
            harvestAnimTimer = 2;

            if (t.crop == Strawberry) {
                inventory.strawberryHarvested++;
                t.stage = Grown;
            } else if (t.crop == Sunflower) {
                inventory.sunflowerHarvested++;
                inventory.sunflowerSeeds += QRandomGenerator::global()->bounded(3);
                t.crop = None;
                t.isPlowed = false;  // 向日葵收割后土地还原
            }
        }
        else if (t.crop == None) {
            // 空土地播种逻辑
            if (player.selectedSeed == Strawberry && inventory.strawberrySeeds > 0 && currentSeason == Spring) {
                inventory.strawberrySeeds--; t.crop = Strawberry; t.stage = Seed; t.growthTimer = 0;
            } else if (player.selectedSeed == Sunflower && inventory.sunflowerSeeds > 0 && (currentSeason == Summer || currentSeason == Fall)) {
                inventory.sunflowerSeeds--; t.crop = Sunflower; t.stage = Seed; t.growthTimer = 0;
            }
            // 【修改说明】这里去掉了原本的取消开垦代码，现在地只要开垦了就不会变回去了！
        }
    }
}

void GameWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);

    QPixmap *currentBg = &img_bgWinter;
    QColor fallbackColor = QColor(200, 200, 220);
    if (currentSeason == Spring) { currentBg = &img_bgSpring; fallbackColor = QColor(150, 220, 150); }
    if (currentSeason == Summer) { currentBg = &img_bgSummer; fallbackColor = QColor(100, 200, 100); }
    if (currentSeason == Fall)   { currentBg = &img_bgFall; fallbackColor = QColor(220, 180, 100); }

    // --- 1. 画背景 ---
    for (int r = 0; r < MAP_ROWS; ++r) {
        for (int c = 0; c < MAP_COLS; ++c) {
            if (!currentBg->isNull()) {
                painter.drawPixmap(c * TILE_W, r * TILE_H, *currentBg);
            } else {
                painter.fillRect(c * TILE_W, r * TILE_H, TILE_W, TILE_H, fallbackColor);
            }
        }
    }

    // --- 2. 画土地和作物 ---
    for (int r = 0; r < MAP_ROWS; ++r) {
        for (int c = 0; c < MAP_COLS; ++c) {
            int px = c * TILE_W;
            int py = r * TILE_H;

            // 画土地
            if (map[r][c].isPlowed) {
                if (!img_soilPlowed.isNull()) painter.drawPixmap(px, py, img_soilPlowed);
                else painter.fillRect(px, py, TILE_W, TILE_H, QColor(139, 69, 19));
            }

            // 画作物
            Tile &t = map[r][c];
            if (t.crop != None) {
                QPixmap *cropImg = nullptr;
                QColor cropFallback = (t.crop == Strawberry) ? Qt::red : Qt::yellow;

                if (t.crop == Strawberry) {
                    if (t.stage == Seed) cropImg = &img_strawSeed;
                    else if (t.stage == Seedling) cropImg = &img_strawSeedling;
                    else if (t.stage == Grown) cropImg = &img_strawGrown;
                    else if (t.stage == Fruiting) cropImg = &img_strawFruiting;
                } else if (t.crop == Sunflower) {
                    if (t.stage == Seed) cropImg = &img_sunSeed;
                    else if (t.stage == Seedling) cropImg = &img_sunSeedling;
                    else if (t.stage == Grown) cropImg = &img_sunGrown;
                    else if (t.stage == Fruiting) cropImg = &img_sunFruiting;
                }

                if (cropImg != nullptr && !cropImg->isNull()) {
                    painter.drawPixmap(px, py, *cropImg);
                } else {
                    painter.setBrush(cropFallback);
                    painter.drawEllipse(px + 4, py + 16, TILE_W - 8, TILE_H/2);
                }
            }
        }
    }

    // --- 3. 画玩家 ---
    QPixmap *playerImg = &img_playerDown;

    // 【修改】如果在收获动画期间，强制使用收获图片
    if (isHarvesting) {
        playerImg = &img_playerHarvest;
    } else {
        if (player.dir == Up) playerImg = &img_playerUp;
        if (player.dir == Down) playerImg = &img_playerDown;
        if (player.dir == Left) playerImg = &img_playerLeft;
        if (player.dir == Right) playerImg = &img_playerRight;
    }

    if (!playerImg->isNull()) {
        painter.drawPixmap(player.x * TILE_W, player.y * TILE_H, *playerImg);
    } else {
        painter.fillRect(player.x * TILE_W, player.y * TILE_H, TILE_W, TILE_H, Qt::blue);
    }

    // --- 4. 绘制UI ---
    drawUI(painter);
    if (isGameOver) drawSettlement(painter);
}


void GameWindow::drawUI(QPainter &painter) {
    // 【修改】把背景框的高度从 140 增加到 165，容纳新增的倒计时文字
    painter.setBrush(QColor(0, 0, 0, 150));
    painter.setPen(Qt::white);
    painter.drawRect(10, 10, 200, 165);

    QString seasonStr = (currentSeason == Winter) ? "Winter (Dig!)" :
                            (currentSeason == Spring) ? "Spring" :
                            (currentSeason == Summer) ? "Summer" : "Fall";
    painter.drawText(20, 30, "Season: " + seasonStr);

    QString seedStr = (player.selectedSeed == Strawberry) ? "[1] Strawberry" : "[2] Sunflower";
    painter.drawText(20, 50, "Hand: " + seedStr);

    painter.drawText(20, 70, QString("Straw. Seeds: %1").arg(inventory.strawberrySeeds));
    painter.drawText(20, 90, QString("Sunf. Seeds: %1").arg(inventory.sunflowerSeeds));
    painter.drawText(20, 110, QString("Straw. Harvests: %1").arg(inventory.strawberryHarvested));
    painter.drawText(20, 130, QString("Sunf. Harvests: %1").arg(inventory.sunflowerHarvested));

    // --- 【新增】倒计时显示 ---
    // 每个 Tick 是 200ms，一秒有 5 个 Tick。满 Tick 是 200。
    int remainingSeconds = (200 - timeTicks) / 5;
    painter.drawText(20, 150, QString("Time Left: %1s").arg(remainingSeconds));

    // 进度条显示季节流逝 (位置稍微下移到 160)
    painter.setBrush(Qt::transparent);
    painter.drawRect(10, 160, 200, 10);
    painter.setBrush(Qt::white);
    painter.drawRect(10, 160, timeTicks, 10); // timeTicks最大200，正好填满长度200的条
}

void GameWindow::drawSettlement(QPainter &painter) {
    painter.setBrush(QColor(0, 0, 0, 200));
    painter.drawRect(rect());
    painter.setPen(Qt::white);
    QFont f = painter.font(); f.setPointSize(24); painter.setFont(f);
    int totalScore = (inventory.strawberryHarvested * 10) + (inventory.sunflowerHarvested * 15);
    painter.drawText(rect(), Qt::AlignCenter, QString("NEXT WINTER HAS COME!\n\nEarnings: $%1").arg(totalScore));
}

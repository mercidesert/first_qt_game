#include "GameWindow.h"
#include <QPainter>
#include <QRandomGenerator>

GameWindow::GameWindow(QWidget *parent)
    : QWidget(parent), currentState(MainMenu),
    currentSeason(Winter), timeTicks(0),
    isHarvesting(false), harvestAnimTimer(0), dialogueTimer(0),
    fishState(FishIdle), isSpaceHeld(false)
{
    setFixedSize(MAP_COLS * TILE_W, MAP_ROWS * TILE_H);
    setWindowTitle("Stadew Farm");

    for (int r = 0; r < MAP_ROWS; ++r) {
        for (int c = 0; c < MAP_COLS; ++c) { map[r][c].isPlowed = false; map[r][c].crop = None; }
    }

    auto loadAndScale =[](const QString& path) { return QPixmap(path).scaled(TILE_W, TILE_H, Qt::IgnoreAspectRatio, Qt::FastTransformation); };
    auto loadBg = [&](const QString& path) { return QPixmap(path).scaled(width(), height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation); };

    img_bgWinter = loadAndScale(":/images/bg_winter.png");
    img_bgSpring = loadAndScale(":/images/bg_spring.png");
    img_bgSummer = loadAndScale(":/images/bg_summer.png");
    img_bgFall   = loadAndScale(":/images/bg_fall.png");
    img_soilPlowed = loadAndScale(":/images/soil_plowed.png");
    img_playerHarvest = loadAndScale(":/images/player_harvest.png");

    QString dirNames[4] = {"up", "down", "left", "right"};
    for (int d = 0; d < 4; ++d) {
        for (int f = 0; f < 4; ++f) {
            img_playerWalk[d][f] = loadAndScale(QString(":/images/player_%1_%2.png").arg(dirNames[d]).arg(f + 1));
        }
    }
    img_strawSeed = loadAndScale(":/images/straw_1_seed.png"); img_strawSeedling = loadAndScale(":/images/straw_2_seedling.png");
    img_strawGrown = loadAndScale(":/images/straw_3_grown.png"); img_strawFruiting = loadAndScale(":/images/straw_4_fruiting.png");
    img_sunSeed = loadAndScale(":/images/sun_1_seed.png"); img_sunSeedling = loadAndScale(":/images/sun_2_seedling.png");
    img_sunGrown = loadAndScale(":/images/sun_3_grown.png"); img_sunFruiting = loadAndScale(":/images/sun_4_fruiting.png");

    // --- 场景背景 ---
    img_titleBg = loadBg(":/images/title.png"); // 加载你找的菜单图
    img_homeBg = loadBg(":/images/home.png");
    img_feastBg = loadBg(":/images/feast.png");
    img_riverBg = loadBg(":/images/river_bg.png");
    img_fishIcon = QPixmap(":/images/fish_icon.png").scaled(30, 30);

    // --- 钓鱼系统 ---
    caughtFishIndex = -1;
    fishNames[0] = "pufferfish"; fishNames[1] = "tuna"; fishNames[2] = "perch";
    fishNames[3] = "catfish"; fishNames[4] = "squid"; fishNames[5] = "seaweed";

    for (int i = 0; i < 6; ++i) {
        img_fishes[i] = QPixmap(QString(":/images/fish_%1.png").arg(i + 1)).scaled(96, 96, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    // 初始化人物
    initNPCs();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GameWindow::gameTick);
    timer->start(200);
}

GameWindow::~GameWindow() {}

// --- 【重点】在这里给每个人个性化定制三句话 ---
void GameWindow::initNPCs() {
    // 升级了添加工具，现在需要直接传入3句不同的话
    auto addNPC = [&](QString name, int x1, int y1, int x2, int y2, QString msg1, QString msg2, QString msg3) {
        NPC npc;
        npc.name = name;
        npc.clickArea = QRect(x1, y1, x2 - x1, y2 - y1);
        npc.dialogues << msg1 << msg2 << msg3; // 把传进来的三句话放进对应人物的对话库里
        npcList.append(npc);
    };

    addNPC("Pierre", 24, 55, 54, 135,
           "如果你需要种子，明天记得来皮埃尔杂货店！",
           "就算过节我也在想着明天的营业额。",
           "其实当年我的拳击技术可是很厉害的。");

    addNPC("Caroline", 21, 153, 55, 220,
           "这是我用温室种的茶叶泡的茶，尝尝吧。",
           "阿比盖尔这孩子又跑到哪里去了？",
           "节日祭典的布置真是花了不少心思。");

    addNPC("Abigail", 176, 109, 204, 177,
           "嘿！你觉得今天我会赢下找冰蛋比赛吗？",
           "其实我觉得下雨天更适合出来逛。",
           "（嚼嚼嚼）这个紫水晶真好吃……");

    addNPC("Shane", 25, 271, 54, 345,
           "……干嘛？别来烦我。",
           "给我来杯啤酒，不然我不想说话。",
           "玛妮阿姨做的披萨味道还不错。");

    addNPC("Clint", 24, 355, 55, 435,
           "我的铁匠铺今天关门了。",
           "如果你有晶石，明天再拿来给我砸吧。",
           "你…你觉得艾米丽今天穿得好看吗？");

    addNPC("Gus", 99, 437, 130, 515,
           "欢迎！星之果实餐吧今天提供免费点心！",
           "看到大家吃得开心，我就满足了。",
           "锅里的汤还要再熬一会儿。");

    addNPC("Elliott", 61, 482, 90, 559,
           "这冬日的雪景，正是绝佳的写作素材啊。",
           "我的头发在风中凌乱了吗？",
           "来一杯红酒，致这美好的夜晚。");

    addNPC("Leah", 134, 488, 164, 558,
           "我喜欢在这个季节雕刻木头。",
           "你尝过那些用野果做的沙拉了吗？",
           "自然的美景总是能给我带来灵感。");

    addNPC("Harvey", 174, 351, 203, 433,
           "最近天气冷，注意保暖别感冒了。",
           "医生也有需要放松的时候呀。",
           "这里的空气真不错，多深呼吸。");

    addNPC("??", 177, 229, 209, 304,
           "……",
           "嘘，别告诉别人我在这里。",
           "你觉得这棵树上的星星亮吗？");

    addNPC("Emily", 285, 444, 323, 513,
           "我能感受到你周围散发着温暖的光环！",
           "这件衣服是我自己做的，好看吧？",
           "要和我一起跳舞吗？");

    addNPC("Haley", 324, 447, 357, 511,
           "哎呀，我的鞋子上沾到泥了。",
           "这种聚会真是太无聊了。",
           "如果你带了向日葵，我可能心情会好一点。");

    addNPC("George", 445, 492, 485, 555,
           "哼，想当年我可是能在雪地里跑一整天的。",
           "音乐太吵了！",
           "艾芙琳做的饼干是这里唯一值得期待的东西。");

    addNPC("Evelyn", 479, 494, 511, 562,
           "星露谷的冬星节总是这么温馨。",
           "来尝尝我做的饼干吧，孩子。",
           "看到你们年轻人精神这么好，真让人高兴。");

    addNPC("Alex", 516, 441, 549, 512,
           "就算过节，我也不会停止锻炼的！",
           "嘿，等天气暖和了，我们来传球吧。",
           "总有一天我会成为职业橄榄球运动员。");

    addNPC("Jodi", 445, 319, 473, 387,
           "总算可以不用做家务，好好放松一下了。",
           "文森特又不知道跑去哪里野了。",
           "真希望肯特现在也能在这里……");

    addNPC("Sam", 595, 308, 621, 385,
           "刚才的音乐你听了吗？太酷了！",
           "我都快饿扁了，什么时候开饭？",
           "等会儿我要和塞巴斯蒂安去打台球。");

    addNPC("Vincent", 517, 249, 549, 286,
           "哥哥不带我玩！",
           "妈妈说吃太多糖牙齿会掉光，可是我想吃！",
           "你看到那边大大的礼物盒了吗？");

    addNPC("Kent", 477, 223, 514, 283,
           "人太多了……让我有些不习惯。",
           "我还在努力适应这里平静的生活。",
           "爆竹的声音有时会让我回想起一些不好的事。");

    addNPC("Maru", 445, 65, 475, 133,
           "我在想能不能发明一个自动送礼物的机器。",
           "今晚的星星一定很漂亮。",
           "你对电子元件感兴趣吗？");

    addNPC("Demetrius", 595, 95, 621, 177,
           "从科学的角度来看，树上的灯光排列很有趣。",
           "我在观察冬星节人群的社交行为。",
           "不知道罗宾会不会喜欢我准备的礼物。");

    addNPC("Robin", 516, 21, 553, 72,
           "这里的木质舞台搭建得真不错，很结实。",
           "需要升级农场建筑随时来找我！",
           "站久了还真有点冷呢。");

    addNPC("Sebastian", 555, 15, 586, 73,
           "……人太多了，我想回房间打游戏。",
           "其实我更喜欢下雨天。",
           "等会儿山姆又要拉着我瞎逛了。");
}

void GameWindow::gameTick() {
    timeTicks++;
    updateCrops();

    if (timeTicks >= 200) {
        timeTicks = 0;
        int next = currentSeason + 1;
        if (next > Fall) currentSeason = Winter;
        else currentSeason = static_cast<Season>(next);

        for (int r=0; r<MAP_ROWS; ++r) for (int c=0; c<MAP_COLS; ++c) {
                if (map[r][c].crop == Strawberry && currentSeason != Spring) map[r][c].crop = None;
                if (map[r][c].crop == Sunflower && currentSeason == Winter) map[r][c].crop = None;
            }
    }

    if (isHarvesting) { harvestAnimTimer--; if (harvestAnimTimer <= 0) isHarvesting = false; }
    if (dialogueTimer > 0) dialogueTimer--;

    if (currentState == Fishing) updateFishing();

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

void GameWindow::updateFishing() {
    auto rng = QRandomGenerator::global();
    if (fishState == FishWaiting) {
        fishTimer--;
        if (fishTimer <= 0) { fishState = FishBiting; fishTimer = 5; }
    }
    else if (fishState == FishBiting) {
        fishTimer--;
        if (fishTimer <= 0) { fishState = FishIdle; dialogueTimer = 0; }
    }
    else if (fishState == FishPlaying) {
        if (isSpaceHeld) barVy -= 2.0f; else barVy += 1.5f;
        barVy *= 0.85f;
        barY += barVy;
        if (barY < 0) { barY = 0; barVy = 0; }
        if (barY > 300 - 50) { barY = 300 - 50; barVy = 0; }

        if (abs(fishY - fishTarget) < 5 || rng->bounded(100) < 5) fishTarget = rng->bounded(250);
        if (fishY < fishTarget) fishY += 3.0f; else fishY -= 3.0f;

        if (fishY >= barY && fishY <= barY + 50) catchProgress += 2.0f;
        else catchProgress -= 1.0f;


        if (catchProgress >= 100) {
            fishState = FishResult;
            caughtFishIndex = QRandomGenerator::global()->bounded(6);
        }
        if (catchProgress <= 0) {
            fishState = FishResult;
            caughtFishIndex = -1;
        }
    }
}

void GameWindow::mousePressEvent(QMouseEvent *event) {
    if (currentState == MainMenu) {
        int mx = event->pos().x(); int my = event->pos().y();
        if (mx >= 220 && mx <= 420) {
            if (my >= 220 && my <= 270) currentState = Home;
            else if (my >= 290 && my <= 340) currentState = Festival;
            else if (my >= 360 && my <= 410) currentState = Farm;
            else if (my >= 430 && my <= 480) { currentState = Fishing; fishState = FishIdle; currentDialogue = ""; }
        }
    }
    else if (currentState == Festival) {
        for (const NPC &npc : npcList) {
            if (npc.clickArea.contains(event->pos())) {
                int randIndex = QRandomGenerator::global()->bounded(npc.dialogues.size());
                currentDialogue = npc.name + ":\n" + npc.dialogues[randIndex];
                dialogueTimer = 25; // 对话气泡显示5秒
                break;
            }
        }
    }

    else if (currentState == Fishing && fishState == FishResult) {
        fishState = FishIdle;
    }
}

void GameWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape && currentState != MainMenu) {
        currentState = MainMenu;
        return;
    }

    if (currentState == Farm) {
        bool moved = false;
        if (event->key() == Qt::Key_W) { player.y = qMax(0, player.y - 1); player.dir = Up; moved = true; }
        if (event->key() == Qt::Key_S) { player.y = qMin(MAP_ROWS - 1, player.y + 1); player.dir = Down; moved = true; }
        if (event->key() == Qt::Key_A) { player.x = qMax(0, player.x - 1); player.dir = Left; moved = true; }
        if (event->key() == Qt::Key_D) { player.x = qMin(MAP_COLS - 1, player.x + 1); player.dir = Right; moved = true; }
        if (moved) player.animFrame = (player.animFrame + 1) % 4;

        if (event->key() == Qt::Key_1) player.selectedSeed = Strawberry;
        if (event->key() == Qt::Key_2) player.selectedSeed = Sunflower;

        if (event->key() == Qt::Key_J || event->key() == Qt::Key_Space) handleFarmAction();
    }
    else if (currentState == Fishing) {
        // 【修改】钓鱼按键逻辑
        if (event->key() == Qt::Key_Space) {
            isSpaceHeld = true;
            if (fishState == FishIdle) {
                fishState = FishWaiting;
                fishTimer = QRandomGenerator::global()->bounded(10, 25);
            } else if (fishState == FishBiting) {
                fishState = FishPlaying;
                barY = 150; barVy = 0; fishY = 150; fishTarget = 150; catchProgress = 30;
            } else if (fishState == FishResult) {
                // 如果在结算画面，按空格继续
                fishState = FishIdle;
            }
        }
    }
    update();
}

void GameWindow::keyReleaseEvent(QKeyEvent *event) {
    if (!event->isAutoRepeat()) {
        if (currentState == Farm) {
            if (event->key() == Qt::Key_W || event->key() == Qt::Key_S || event->key() == Qt::Key_A || event->key() == Qt::Key_D) {
                player.animFrame = 0; update();
            }
        } else if (currentState == Fishing) {
            if (event->key() == Qt::Key_Space) isSpaceHeld = false;
        }
    }
}

void GameWindow::handleFarmAction() {
    int tx = player.x; int ty = player.y;
    if (player.dir == Up) ty--; else if (player.dir == Down) ty++; else if (player.dir == Left) tx--; else if (player.dir == Right) tx++;
    if (tx < 0 || tx >= MAP_COLS || ty < 0 || ty >= MAP_ROWS) return;

    Tile &t = map[ty][tx];
    if (!t.isPlowed) {
        t.isPlowed = true;
        int rng = QRandomGenerator::global()->bounded(100);
        if (rng < 10) inventory.strawberrySeeds++; else if (rng < 35) inventory.sunflowerSeeds++;
    } else {
        if (t.crop != None && t.stage == Fruiting) {
            isHarvesting = true; harvestAnimTimer = 2;
            if (t.crop == Strawberry) { inventory.strawberryHarvested++; t.stage = Grown; }
            else if (t.crop == Sunflower) { inventory.sunflowerHarvested++; inventory.sunflowerSeeds += QRandomGenerator::global()->bounded(3); t.crop = None; t.isPlowed = false; }
        } else if (t.crop == None) {
            if (player.selectedSeed == Strawberry && inventory.strawberrySeeds > 0 && currentSeason == Spring) {
                inventory.strawberrySeeds--; t.crop = Strawberry; t.stage = Seed; t.growthTimer = 0;
            } else if (player.selectedSeed == Sunflower && inventory.sunflowerSeeds > 0 && (currentSeason == Summer || currentSeason == Fall)) {
                inventory.sunflowerSeeds--; t.crop = Sunflower; t.stage = Seed; t.growthTimer = 0;
            }
        }
    }
}

void GameWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);

    auto drawHint =[&]() {
        painter.setPen(Qt::white);
        painter.drawText(10, 20, "Press 'ESC' to return to Main Menu");
    };

    if (currentState == MainMenu) drawMainMenu(painter);
    else if (currentState == Home) { drawHome(painter); drawHint(); }
    else if (currentState == Festival) { drawFestival(painter); drawHint(); }
    else if (currentState == Farm) { drawFarm(painter); drawHint(); }
    else if (currentState == Fishing) { drawFishing(painter); drawHint(); }
}

// --- 【修改】无边框字体风格的菜单 ---
void GameWindow::drawMainMenu(QPainter &painter) {
    if(!img_titleBg.isNull()) painter.drawPixmap(0, 0, img_titleBg);
    else painter.fillRect(rect(), QColor(50, 80, 120));

    // 使用带有加粗、类似像素/卡通风格的系统自带字体
    QFont f("Comic Sans MS", 22, QFont::Black);
    painter.setFont(f);

    QString menus[4] = {"Go Home", "Town Festival", "Farm", "River (Fishing)"};
    for(int i=0; i<4; i++) {
        QRect btn(220, 220 + i*70, 200, 50);

        // 增加字体黑色阴影，让白色字体在任何背景上都能看清
        painter.setPen(Qt::black);
        painter.drawText(btn.adjusted(2, 2, 2, 2), Qt::AlignCenter, menus[i]);

        painter.setPen(QColor(255, 245, 220)); // 星露谷风格的暖白色字体
        painter.drawText(btn, Qt::AlignCenter, menus[i]);
    }
}

void GameWindow::drawHome(QPainter &painter) {
    if(!img_homeBg.isNull()) painter.drawPixmap(0, 0, img_homeBg);
    else painter.fillRect(rect(), Qt::darkGray);
}

void GameWindow::drawFestival(QPainter &painter) {
    if(!img_feastBg.isNull()) painter.drawPixmap(0, 0, img_feastBg);
    else painter.fillRect(rect(), Qt::darkGreen);

    // 【修改】已经去除了这里的红色辅助框！

    if (dialogueTimer > 0 && !currentDialogue.isEmpty()) {
        painter.setBrush(Qt::white);
        painter.setPen(Qt::black);
        painter.drawRect(100, 500, 440, 80);
        painter.drawText(QRect(110, 510, 420, 60), Qt::AlignLeft | Qt::TextWordWrap, currentDialogue);
    }
}

void GameWindow::drawFarm(QPainter &painter) {
    QPixmap *currentBg = &img_bgWinter;
    QColor fallbackColor = QColor(200, 200, 220);
    if (currentSeason == Spring) { currentBg = &img_bgSpring; fallbackColor = QColor(150, 220, 150); }
    if (currentSeason == Summer) { currentBg = &img_bgSummer; fallbackColor = QColor(100, 200, 100); }
    if (currentSeason == Fall)   { currentBg = &img_bgFall; fallbackColor = QColor(220, 180, 100); }

    for (int r = 0; r < MAP_ROWS; ++r) {
        for (int c = 0; c < MAP_COLS; ++c) {
            if (!currentBg->isNull()) painter.drawPixmap(c * TILE_W, r * TILE_H, *currentBg);
            else painter.fillRect(c * TILE_W, r * TILE_H, TILE_W, TILE_H, fallbackColor);
        }
    }

    for (int r = 0; r < MAP_ROWS; ++r) {
        for (int c = 0; c < MAP_COLS; ++c) {
            int px = c * TILE_W; int py = r * TILE_H;
            if (map[r][c].isPlowed) {
                if (!img_soilPlowed.isNull()) painter.drawPixmap(px, py, img_soilPlowed);
                else painter.fillRect(px, py, TILE_W, TILE_H, QColor(139, 69, 19));
            }
            Tile &t = map[r][c];
            if (t.crop != None) {
                QPixmap *cropImg = nullptr;
                QColor cropFallback = (t.crop == Strawberry) ? Qt::red : Qt::yellow;
                if (t.crop == Strawberry) {
                    if (t.stage == Seed) cropImg = &img_strawSeed; else if (t.stage == Seedling) cropImg = &img_strawSeedling;
                    else if (t.stage == Grown) cropImg = &img_strawGrown; else if (t.stage == Fruiting) cropImg = &img_strawFruiting;
                } else if (t.crop == Sunflower) {
                    if (t.stage == Seed) cropImg = &img_sunSeed; else if (t.stage == Seedling) cropImg = &img_sunSeedling;
                    else if (t.stage == Grown) cropImg = &img_sunGrown; else if (t.stage == Fruiting) cropImg = &img_sunFruiting;
                }
                if (cropImg != nullptr && !cropImg->isNull()) painter.drawPixmap(px, py, *cropImg);
                else { painter.setBrush(cropFallback); painter.drawEllipse(px + 4, py + 16, TILE_W - 8, TILE_H/2); }
            }
        }
    }

    QPixmap *playerImg = nullptr;
    if (isHarvesting) playerImg = &img_playerHarvest;
    else playerImg = &img_playerWalk[player.dir][player.animFrame];

    if (playerImg != nullptr && !playerImg->isNull()) painter.drawPixmap(player.x * TILE_W, player.y * TILE_H, *playerImg);
    else painter.fillRect(player.x * TILE_W, player.y * TILE_H, TILE_W, TILE_H, Qt::blue);

    painter.setBrush(QColor(0, 0, 0, 150)); painter.setPen(Qt::white);
    painter.drawRect(10, 30, 200, 165);
    QString seasonStr = (currentSeason == Winter) ? "Winter" : (currentSeason == Spring) ? "Spring" : (currentSeason == Summer) ? "Summer" : "Fall";
    painter.drawText(20, 50, "Season: " + seasonStr);
    QString seedStr = (player.selectedSeed == Strawberry) ? "[1] Strawberry" : "[2] Sunflower";
    painter.drawText(20, 70, "Hand: " + seedStr);
    painter.drawText(20, 90, QString("Straw. Seeds: %1").arg(inventory.strawberrySeeds));
    painter.drawText(20, 110, QString("Sunf. Seeds: %1").arg(inventory.sunflowerSeeds));
    painter.drawText(20, 130, QString("Straw. Harvests: %1").arg(inventory.strawberryHarvested));
    painter.drawText(20, 150, QString("Sunf. Harvests: %1").arg(inventory.sunflowerHarvested));
    painter.drawText(20, 170, QString("Time Left: %1s").arg((200 - timeTicks) / 5));
    painter.setBrush(Qt::white); painter.drawRect(10, 180, timeTicks, 10);
}

// --- 【修改】静态等待继续的钓鱼结算 ---
void GameWindow::drawFishing(QPainter &painter) {
    if(!img_riverBg.isNull()) painter.drawPixmap(0, 0, img_riverBg);
    else painter.fillRect(rect(), QColor(50, 150, 200));

    painter.setPen(Qt::white);
    QFont f = painter.font(); f.setPointSize(16); f.setBold(true); painter.setFont(f);

    if (fishState == FishIdle) {
        painter.drawText(rect(), Qt::AlignCenter, "Press SPACE to cast line");
    } else if (fishState == FishWaiting) {
        painter.drawText(rect(), Qt::AlignCenter, "Waiting...");
    } else if (fishState == FishBiting) {
        painter.setPen(Qt::red); f.setPointSize(64); painter.setFont(f);
        painter.drawText(rect(), Qt::AlignCenter, "!");
    } else if (fishState == FishPlaying) {
        int gameX = 500; int gameY = 150;
        painter.setBrush(QColor(0, 0, 0, 150)); painter.drawRect(gameX, gameY, 40, 300);
        painter.setBrush(QColor(0, 255, 0, 180)); painter.drawRect(gameX, gameY + barY, 40, 50);

        if (!img_fishIcon.isNull()) painter.drawPixmap(gameX + 5, gameY + fishY, img_fishIcon);
        else { painter.setBrush(Qt::red); painter.drawEllipse(gameX + 10, gameY + fishY, 20, 20); }

        painter.setBrush(Qt::transparent); painter.setPen(Qt::white);
        painter.drawRect(gameX + 50, gameY, 15, 300);
        painter.setBrush(Qt::green);
        painter.drawRect(gameX + 50, gameY + 300 - (catchProgress * 3), 15, catchProgress * 3);
    }
    // 钓鱼结算悬停画面
    else if (fishState == FishResult) {
        // 画一个半透明黑底，突出结算画面
        painter.setBrush(QColor(0,0,0,150));
        painter.drawRect(rect());

        if (caughtFishIndex != -1) {
            // 画鱼
            if (!img_fishes[caughtFishIndex].isNull()) {
                painter.drawPixmap(320 - 48, 250, img_fishes[caughtFishIndex]);
            }

            // 【重点】这里修改这行文字内容！
            QString customText = QString("Great！You caught a 【%1】 ！！").arg(fishNames[caughtFishIndex]);

            painter.setPen(Qt::yellow); f.setPointSize(24); painter.setFont(f);
            painter.drawText(QRect(0, 180, 640, 50), Qt::AlignCenter, "CATCH!");

            painter.setPen(Qt::white); f.setPointSize(16); painter.setFont(f);
            painter.drawText(QRect(0, 380, 640, 40), Qt::AlignCenter, customText);
        } else {
            painter.setPen(Qt::white); f.setPointSize(24); painter.setFont(f);
            painter.drawText(QRect(0, 280, 640, 50), Qt::AlignCenter, "The fish said 'Loser!hahah'...");
        }

        // 提示按键继续
        f.setPointSize(12); painter.setFont(f);
        painter.setPen(Qt::lightGray);
        painter.drawText(QRect(0, 500, 640, 40), Qt::AlignCenter, "-- Press space or click to continue --");
    }
}

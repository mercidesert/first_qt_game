#include "GameWindow.h"
#include <QPainter>
#include <QRandomGenerator>

GameWindow::GameWindow(QWidget *parent)
    : QWidget(parent), currentState(MainMenu),
    currentSeason(Winter), timeTicks(0),
    isHarvesting(false), harvestAnimTimer(0), dialogueTimer(0),
    abigailClickCount(0), pierreClickCount(0), inShop(false),
    fishState(FishIdle), isSpaceHeld(false), hasTreasure(false), treasureState(0),
    homeState(HomeNormal), blindBoxIndex(0), fastAnimTimer(0), fastAnimFrame(0),
    emoteState(0), emoteTimer(0), emoteId(0)
//状态机定义
{
    setFixedSize(MAP_COLS * TILE_W, MAP_ROWS * TILE_H);
    setWindowTitle("星露农场 (简陋的简易版星露谷)");

    gold = 150;
    maxEnergy = 100;
    energy = 100;
    globalMsgTimer = 0;
    isInjured = false;
    harveyClickCount = 0;
    combatLevel = 1;
    exitBtnRect = QRect(15, 570, 64, 54);  // 体力值背包等设置，统一退出键区域

    for (int r = 0; r < MAP_ROWS; ++r) {
        for (int c = 0; c < MAP_COLS; ++c) { map[r][c].isPlowed = false; map[r][c].crop = None; }
    }

    auto loadAndScale =[](const QString& path) { return QPixmap(path).scaled(TILE_W, TILE_H, Qt::IgnoreAspectRatio, Qt::FastTransformation); };
    auto loadBg = [&](const QString& path) { return QPixmap(path).scaled(width(), height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation); };

    img_bgWinter = loadAndScale(":/images/bg_winter.png"); img_bgSpring = loadAndScale(":/images/bg_spring.png");
    img_bgSummer = loadAndScale(":/images/bg_summer.png"); img_bgFall   = loadAndScale(":/images/bg_fall.png");
    img_soilPlowed = loadAndScale(":/images/soil_plowed.png"); img_playerHarvest = loadAndScale(":/images/player_harvest.png");

    QString dirNames[4] = {"up", "down", "left", "right"};
    for (int d = 0; d < 4; ++d) {
        for (int f = 0; f < 4; ++f) { img_playerWalk[d][f] = loadAndScale(QString(":/images/player_%1_%2.png").arg(dirNames[d]).arg(f + 1)); }
    }
    //玩家动态展示
    img_strawSeed = loadAndScale(":/images/straw_1_seed.png"); img_strawSeedling = loadAndScale(":/images/straw_2_seedling.png");
    img_strawGrown = loadAndScale(":/images/straw_3_grown.png"); img_strawFruiting = loadAndScale(":/images/straw_4_fruiting.png");
    img_sunSeed = loadAndScale(":/images/sun_1_seed.png"); img_sunSeedling = loadAndScale(":/images/sun_2_seedling.png");
    img_sunGrown = loadAndScale(":/images/sun_3_grown.png"); img_sunFruiting = loadAndScale(":/images/sun_4_fruiting.png");
    //植物生长状态加载
    img_titleBg = loadBg(":/images/title.png"); img_homeBg = loadBg(":/images/home.png");
    img_feastBg = loadBg(":/images/feast.png"); img_riverBg = loadBg(":/images/river_bg.png");
    img_fishIcon = QPixmap(":/images/fish_icon.png").scaled(30, 30); img_envelope = QPixmap(":/images/envelope.png");
    img_enemy = QPixmap(":/images/enemy.png").scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    img_note = QPixmap(":/images/note.png").scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    img_easterBg = loadBg(":/images/eastern.png");
    img_egg = QPixmap(":/images/egg.png").scaled(12, 12, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    img_combatBg = loadBg(":/images/combat_bg.png");
    img_textbox = loadBg(":/images/textbox.png");
    img_exit = QPixmap(":/images/exit.png").scaled(64, 54, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    img_boxClosed = QPixmap(":/images/box_closed.png").scaled(48, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    for(int i=0; i<5; i++) img_boxOpen[i] = QPixmap(QString(":/images/box_o%1.png").arg(i+1)).scaled(48, 64, Qt::KeepAspectRatio);
    for(int i=0; i<4; i++) img_bird[i] = QPixmap(QString(":/images/bird_%1.png").arg(i+1)).scaled(64, 64, Qt::KeepAspectRatio);
    for(int i=0; i<4; i++) img_junimo[i] = QPixmap(QString(":/images/junimo_%1.png").arg(i+1)).scaled(64, 64, Qt::KeepAspectRatio);
    for(int i=0; i<4; i++) img_emoAnim[i] = QPixmap(QString(":/images/emo_%1.png").arg(i+1)).scaled(32, 32, Qt::KeepAspectRatio);
    for(int i=0; i<15; i++) img_emos[i] = QPixmap(QString(":/images/emo%1.png").arg(i+1)).scaled(32, 32, Qt::KeepAspectRatio);
    //各类图片加载展示
    // 加载6张装修盲盒图
    for(int i=0; i<6; i++) {
        img_homeVariants[i] = loadBg(QString(":/images/home_%1.png").arg(i+1));
    }

    //6个盲盒设定完全不一样的话语
    homeMessages[0] = "骚动的绿，溪水从几何般的玫瑰中逃出来，打了个很有爱的盹";
    homeMessages[1] = "水在月光中跪下，灯火四肢沉落，向草地匍匐。当我抵达河边，满月覆盖着它";
    homeMessages[2] = "我相信自己，生来如同璀璨的夏花，不凋不败，妖冶如火";
    homeMessages[3] = "你来人间一趟，你要看看太阳";
    homeMessages[4] = "丛林再披青装，又是一年最美好的时光";
    homeMessages[5] = "走向冬天，唱一支歌吧。不祝福，也不祈祷";

    caughtFishIndex = -1;
    fishNames[0] = "河豚"; fishNames[1] = "金枪鱼"; fishNames[2] = "河鲈";
    fishNames[3] = "鲇鱼"; fishNames[4] = "鱿鱼"; fishNames[5] = "海草";
    for (int i = 0; i < 6; ++i) img_fishes[i] = QPixmap(QString(":/images/fish_%1.png").arg(i + 1)).scaled(96, 96, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    initNPCs();

    timer = new QTimer(this); connect(timer, &QTimer::timeout, this, &GameWindow::gameTick); timer->start(200);
    fastTimer = new QTimer(this); connect(fastTimer, &QTimer::timeout, this, &GameWindow::fastTick); fastTimer->start(33);
}

GameWindow::~GameWindow() {}

void GameWindow::initNPCs() {
    auto addNPC = [&](QString name, int x1, int y1, int x2, int y2, QString msg1, QString msg2, QString msg3) {
        NPC npc; npc.name = name; npc.clickArea = QRect(x1, y1, x2 - x1, y2 - y1); npc.dialogues << msg1 << msg2 << msg3; npcList.append(npc);
    };
    addNPC("Pierre", 24, 55, 54, 135, "如果你需要种子，明天记得来杂货店！", "就算过节我也在想着明天的营业额。", "其实当年我的拳击技术很厉害。");
    addNPC("Caroline", 21, 153, 55, 220, "这是我用温室种的茶叶泡的茶，尝尝吧。", "阿比盖尔这孩子又跑到哪里去了？", "节日祭典的布置真是花了不少心思。");
    addNPC("Abigail", 176, 109, 204, 177, "嘿！你觉得今天我会赢下找彩蛋比赛吗？", "其实我觉得下雨天更适合出来逛。", "你怎么知道我饿了？！！");
    addNPC("Shane", 25, 271, 54, 345, "……干嘛？别来烦我。", "给我来杯啤酒，不然我不想说话。", "有时我会意识到自己不过是一副柔弱的血肉之躯。");
    addNPC("Clint", 24, 355, 55, 435, "我的铁匠铺今天关门了。", "如果你有晶石，明天再拿来给我砸吧。", "你…你觉得艾米丽今天穿得好看吗？");
    addNPC("Gus", 99, 437, 130, 515, "欢迎！星之果实餐吧今天提供免费点心！", "看到大家吃得开心，我就满足了。", "锅里的汤还要再熬一会儿。");
    addNPC("Elliott", 61, 482, 90, 559, "这冬日的雪景，正是绝佳的写作素材。", "我的头发在风中凌乱了吗？", "来一杯红酒，致这美好的夜晚。");
    addNPC("Leah", 134, 488, 164, 558, "我喜欢在这个季节雕刻木头。", "你尝过那些用野果做的沙拉了吗？", "自然的美景总是能给我带来灵感。");
    addNPC("Harvey", 174, 351, 203, 433, "最近天气冷，注意保暖别感冒了。", "医生也有需要放松的时候呀。", "这里的空气真不错，多深呼吸。");
    addNPC("??", 177, 229, 209, 304, "……", "嘘，别告诉别人我在这里。", "你觉得这棵树上的星星亮吗？");
    addNPC("Emily", 285, 444, 323, 513, "我能感受到你散发着温暖的光环！", "这件衣服是我自己做的，好看吧？", "要和我一起跳舞吗？");
    addNPC("Haley", 324, 447, 357, 511, "哎呀，我的鞋子上沾到泥了。", "这种聚会真是太无聊了。", "如果你带了向日葵，我可能心情会好一点。");
    addNPC("George", 445, 492, 485, 555, "哼，想当年我可是能在雪地里跑一天的。", "音乐太吵了！", "艾芙琳做的饼干是这里唯一值得期待的。");
    addNPC("Evelyn", 479, 494, 511, 562, "亲爱的，生命短暂，珍惜每一刻。", "来尝尝我做的饼干吧，孩子。", "看到你们年轻人精神这么好，真让人高兴。");
    addNPC("Alex", 516, 441, 549, 512, "就算过节，我也不会停止锻炼的！", "难以置信夏天就这么结束了。", "总有一天我会成为职业橄榄球运动员。");
    addNPC("Jodi", 445, 319, 473, 387, "总算可以不用做家务，好好放松一下了。", "文森特又不知道跑去哪里野了。", "真希望肯特现在也能在这里……");
    addNPC("Sam", 595, 308, 621, 385, "刚才的音乐你听了吗？太酷了！", "我都快饿扁了，什么时候开饭？", "等会儿我要和塞巴斯蒂安去打台球。");
    addNPC("Vincent", 517, 249, 549, 286, "哥哥不带我玩！", "妈妈说吃太多糖牙齿会坏，可是我想吃！", "你看到那边大大的礼物盒了吗？");
    addNPC("Kent", 477, 223, 514, 283, "人太多了……让我有些不习惯。", "我还在努力适应这里平静的生活。", "爆竹的声音有时会让我回想起不好的事。");
    addNPC("Maru", 445, 65, 475, 133, "我在想能不能发明一个自动送礼物的机器。", "今晚的星星一定很漂亮。", "你对电子元件感兴趣吗？");
    addNPC("Demetrius", 595, 95, 621, 177, "从科学的角度来看，树上的灯光排列很有趣。", "我在观察冬星节人群的社交行为。", "不知道罗宾会不会喜欢我准备的礼物。");
    addNPC("Robin", 516, 21, 553, 72, "这里的木质舞台搭建得真不错，很结实。", "需要升级农场建筑随时来找我！", "生活中简单的东西最棒了，就像一阵柔和的风");
    addNPC("Sebastian", 555, 15, 586, 73, "……人太多了，我想回房间打游戏。", "其实我更喜欢下雨天。", "我想一个人出去闯荡，和我的摩托。");
}


//种植变化逻辑
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
    if (globalMsgTimer > 0) globalMsgTimer--;

    if (currentState == Fishing) updateFishing();
    if (currentState == Easter) updateEaster();
}

void GameWindow::fastTick() {
    // 全局基础动画时钟 (鸟和祝尼魔)
    fastAnimTimer++;
    if (fastAnimTimer >= 4) {
        fastAnimTimer = 0;
        fastAnimFrame = (fastAnimFrame + 1) % 4;
    }

    // 钓鱼界面宝箱开启帧动画
    if (currentState == Fishing && fishState == FishResult && hasTreasure && treasureState >= 1 && treasureState <= 5) {
        treasureTimer++;
        if (treasureTimer > 3) {
            treasureState++;
            treasureTimer = 0;
            if (treasureState == 6) {
                if (treasureRewardType == 0) gold += treasureRewardAmount;
                else if (treasureRewardType == 1) inventory.strawberrySeeds += treasureRewardAmount;
                else inventory.sunflowerSeeds += treasureRewardAmount;
            }
        }
    }

    // 表情冒泡动画
    if (currentState == Festival) {
        if (emoteState >= 1 && emoteState <= 4) {
            emoteTimer++;
            if (emoteTimer >= 3) { emoteState++; emoteTimer = 0; }
        } else if (emoteState == 5) {
            emoteTimer++;
            if (emoteTimer >= 45) { emoteState = 0; } // 停留1.5秒
        }
    }

    if (currentState == Combat) updateCombat();
    update();
}


//复活节彩蛋游戏
void GameWindow::initEasterGame() {
    abigailClickCount = 0; easterEggsTotal = 12; easterEggsFound = 0; easterTimeLeft = 75; easterEnded = false; easterEggs.clear();
    auto rng = QRandomGenerator::global();
    for (int i = 0; i < easterEggsTotal; ++i) {
        int ex = rng->bounded(20, 600);
        int ey = rng->bounded(120, 600);
        easterEggs.append({QRect(ex, ey, 12, 12), false});
    }
}

void GameWindow::updateEaster() {
    if (easterEnded) return;
    easterTimeLeft--;
    if (easterTimeLeft <= 0) { easterTimeLeft = 0; easterEnded = true; }
    if (easterEggsFound >= easterEggsTotal) { easterEnded = true; }
}


//战斗界面设置
void GameWindow::initCombat(bool nextLevel) {
    playerHp = 100;
    if (!nextLevel) combatLevel = 1;
    maxEnemyHp = 100 + (combatLevel - 1) * 35;
    enemyHp = maxEnemyHp;

    combatNotes.clear();
    combatSpawnTimer = 30;
    combatFeedback = QString("LEVEL %1 READY!").arg(combatLevel);
    combatFeedbackTimer = 30;
    combatEnded = false;
}

void GameWindow::updateCombat() {
    if (combatEnded) return;
    if (combatFeedbackTimer > 0) combatFeedbackTimer--;

    float speed = 12.0f + combatLevel * 3.5f;
    for (int i = 0; i < combatNotes.size(); ++i) combatNotes[i].x -= speed;

    for (int i = combatNotes.size() - 1; i >= 0; --i) {
        if (combatNotes[i].x < 50) {
            combatNotes.removeAt(i); playerHp -= 10; combatFeedback = "MISS!"; combatFeedbackTimer = 20;
        }
    }

    combatSpawnTimer--;
    //战斗难度无限提升逻辑
    if (combatSpawnTimer <= 0) {
        int rngVal = QRandomGenerator::global()->bounded(100);
        int doubleChance = qMin(60, 10 + combatLevel * 8);

        if (rngVal < doubleChance) {
            combatNotes.append(RhythmNote{ 640.0f, 0 });
            combatNotes.append(RhythmNote{ 640.0f, 1 });
        } else if (rngVal < 50 + doubleChance / 2) {
            combatNotes.append(RhythmNote{ 640.0f, 0 });
        } else {
            combatNotes.append(RhythmNote{ 640.0f, 1 });
        }

        int minSpawn = qMax(5, 20 - combatLevel * 2);
        int maxSpawn = qMax(12, 35 - combatLevel * 3);
        combatSpawnTimer = QRandomGenerator::global()->bounded(minSpawn, maxSpawn);
    }

    if (playerHp <= 0) {
        playerHp = 0; combatEnded = true;
        isInjured = true;
        globalMsg = "你受了重伤！必须找 Harvey 治疗！";
        globalMsgTimer = 30;
    }

    if (enemyHp <= 0 && !combatEnded) {
        enemyHp = 0; combatEnded = true;
        int reward = 10 + combatLevel * 10;
        gold += reward;
        globalMsg = QString("战斗胜利！获得 %1 G").arg(reward);
        globalMsgTimer = 15;
    }
}

//四季变换时的农田逻辑
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

//钓鱼游戏
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
        barVy *= 0.85f; barY += barVy;
        if (barY < 0) { barY = 0; barVy = 0; }
        if (barY > 300 - 50) { barY = 300 - 50; barVy = 0; }

        if (abs(fishY - fishTarget) < 5 || rng->bounded(100) < 5) fishTarget = rng->bounded(250);
        if (fishY < fishTarget) fishY += 3.0f; else fishY -= 3.0f;

        if (fishY >= barY && fishY <= barY + 50) catchProgress += 2.0f;
        else catchProgress -= 1.0f;

        if (catchProgress >= 100) {
            fishState = FishResult;
            caughtFishIndex = rng->bounded(6);
            inventory.fishes[caughtFishIndex]++;

            //随机宝箱掉落判定 (50%概率)
            if (rng->bounded(100) < 50) {
                hasTreasure = true;
                treasureState = 0;
                treasureRewardType = rng->bounded(3); // 0=金币, 1=草莓种子, 2=向日葵种子
                treasureRewardAmount = (treasureRewardType == 0) ? rng->bounded(50, 150) : rng->bounded(1, 4);
            } else hasTreasure = false;

        }
        if (catchProgress <= 0) { fishState = FishResult; caughtFishIndex = -1; hasTreasure = false; }
    }
}

//全局退出键设置
void GameWindow::mousePressEvent(QMouseEvent *event) {
    if (currentState != MainMenu && exitBtnRect.contains(event->pos())) {
        if (currentState == Festival && inShop) inShop = false;
        else if (currentState == Home && homeState != HomeNormal) homeState = HomeNormal;
        else currentState = MainMenu;
        return;
    }

    if (currentState == MainMenu) {
        int mx = event->pos().x(); int my = event->pos().y();
        if (mx >= 220 && mx <= 420) {
            if (my >= 250 && my <= 300) currentState = Home;
            else if (my >= 320 && my <= 370) currentState = Festival;
            else if (my >= 390 && my <= 440) currentState = Farm;
            else if (my >= 460 && my <= 510) { currentState = Fishing; fishState = FishIdle; currentDialogue = ""; }
            else if (my >= 530 && my <= 580) {
                if (isInjured) { globalMsg = "你受伤了，快去找 Harvey 治疗！"; globalMsgTimer = 15; return; }
                if (energy >= 10) { energy -= 10; currentState = Combat; initCombat(false); }
                else { globalMsg = "体力不足！进战斗需要 10 体力！"; globalMsgTimer = 15; }
            }
        }
    }
    else if (currentState == Home) {
        if (homeState == HomeTVShow) {
            QRect returnRect(305, 282, 28, 73);
            if (returnRect.contains(event->pos())) homeState = HomeNormal;
            return;
        }

        QRect tvRect(410, 335, 105, 118);
        QRect tableRect(21, 179, 59, 88);
        QRect bedRect(517, 127, 103, 143);

        if (tableRect.contains(event->pos())) {
            int earned = (inventory.strawberryHarvested * 20) + (inventory.sunflowerHarvested * 15);
            for(int i=0; i<6; i++) { earned += inventory.fishes[i] * 20; inventory.fishes[i] = 0; }
            inventory.strawberryHarvested = 0; inventory.sunflowerHarvested = 0;
            if (earned > 0) {
                gold += earned;
                globalMsg = QString("已出售所有物品，获得 %1 G！").arg(earned);
            } else {
                globalMsg = "背包空空如也，没有什么可以卖的。";
            }
            globalMsgTimer = 15;
        }
        else if (bedRect.contains(event->pos())) {
            energy = maxEnergy;
            globalMsg = "没有早八，睡了个好觉，体力拉满了！";
            globalMsgTimer = 15;
        }
        else if (tvRect.contains(event->pos())) {
            homeState = HomeTVPrompt;
        }
    }

    //节日界面点击区域判定
    else if (currentState == Festival) {
        if (inShop) return;

        QRect emoteRect(285, 55, 72, 45);
        if (emoteRect.contains(event->pos()) && emoteState == 0) {
            emoteState = 1;
            emoteTimer = 0;
            emoteId = QRandomGenerator::global()->bounded(15);
            return;
        }

        for (int i = 0; i < npcList.size(); ++i) {
            const NPC &npc = npcList[i];
            if (npc.clickArea.contains(event->pos())) {
                if (isInjured && npc.name.contains("Harvey")) {
                    harveyClickCount++;
                    if (harveyClickCount >= 2) {
                        if (gold >= 50) {
                            gold -= 50; isInjured = false; harveyClickCount = 0;
                            globalMsg = "治疗成功！花费了 50 G。"; globalMsgTimer = 15;
                            currentDialogue = npc.name + ":\n你的伤已经处理好了，以后不要那么勉强自己了。";
                        } else {
                            currentDialogue = npc.name + ":\n抱歉，你需要支付 50 G 才能进行治疗，去卖点东西吧。";
                        }
                    } else {
                        currentDialogue = npc.name + ":\n你看起来受伤了？请让我来帮帮忙吧。";
                    }
                    dialogueTimer = 25; return;
                }

                if (npc.name == "Abigail") {
                    abigailClickCount++;
                    if (abigailClickCount > 3) { currentState = Easter; initEasterGame(); return; }
                }
                if (npc.name == "Pierre") {
                    pierreClickCount++;
                    if (pierreClickCount >= 3) { inShop = true; return; }
                }

                int randIndex = QRandomGenerator::global()->bounded(npc.dialogues.size());
                currentDialogue = npc.name + ":\n" + npc.dialogues[randIndex];
                dialogueTimer = 25;
                break;
            }
        }
    }
    else if (currentState == Easter) {
        if (!easterEnded) {
            for (int i = 0; i < easterEggs.size(); ++i) {
                if (!easterEggs[i].found && easterEggs[i].rect.contains(event->pos())) {
                    easterEggs[i].found = true; easterEggsFound++; break;
                }
            }
        }
    }
    else if (currentState == Fishing && fishState == FishResult) {
        if (hasTreasure && treasureState == 0) {
            treasureState = 1;
        } else if (!hasTreasure || treasureState >= 6) {
            fishState = FishIdle;
        }
    }
}

//主菜单程序界面
void GameWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape && currentState != MainMenu) {
        if (currentState == Festival && inShop) { inShop = false; return; }
        currentState = MainMenu;
        return;
    }

    if (currentState == Home && homeState == HomeTVPrompt) {
        if (event->key() == Qt::Key_Y) {
            if (gold >= 100) {
                gold -= 100; blindBoxIndex = QRandomGenerator::global()->bounded(6); homeState = HomeTVShow;
            } else { globalMsg = "金币不足！"; globalMsgTimer = 15; homeState = HomeNormal; }
        } else if (event->key() == Qt::Key_N) { homeState = HomeNormal; }
        return;
    }

    if (currentState == Festival && inShop) {
        if (event->key() == Qt::Key_1) {
            if (gold >= 20) { gold -= 20; inventory.strawberrySeeds++; globalMsg = "购买草莓种子成功！"; globalMsgTimer=10; }
            else { globalMsg = "金币不足！"; globalMsgTimer=10; }
        } else if (event->key() == Qt::Key_2) {
            if (gold >= 10) { gold -= 10; inventory.sunflowerSeeds++; globalMsg = "购买向日葵种子成功！"; globalMsgTimer=10; }
            else { globalMsg = "金币不足！"; globalMsgTimer=10; }
        }
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
        if (event->key() == Qt::Key_Space) {
            isSpaceHeld = true;
            if (fishState == FishIdle) {
                if (isInjured) { globalMsg = "你受伤了，必须找 Harvey 治疗！"; globalMsgTimer = 15; return; }
                if (energy >= 5) {
                    energy -= 5; fishState = FishWaiting; fishTimer = QRandomGenerator::global()->bounded(10, 25);
                } else { globalMsg = "体力不够了..."; globalMsgTimer = 15; }
            } else if (fishState == FishBiting) {
                fishState = FishPlaying; barY = 150; barVy = 0; fishY = 150; fishTarget = 150; catchProgress = 30;
            } else if (fishState == FishResult) {
                if (hasTreasure && treasureState == 0) treasureState = 1;
                else if (!hasTreasure || treasureState >= 6) fishState = FishIdle;
            }
        }
    }
    else if (currentState == Combat) {
        if (combatEnded) {
            if (enemyHp <= 0) {
                if (event->key() == Qt::Key_Y) { combatLevel++; initCombat(true); }
                else if (event->key() == Qt::Key_N) { currentState = MainMenu; }
            } else {
                if (event->key() == Qt::Key_Space) currentState = MainMenu;
            }
            return;
        }

        auto processHit = [&](int targetType) {
            bool hit = false;
            for (int i = 0; i < combatNotes.size(); ++i) {
                if (combatNotes[i].type != targetType) continue;
                float diff = std::abs(combatNotes[i].x - 150.0f);
                if (diff <= 30.0f) { combatFeedback = "PERFECT!!"; enemyHp -= 15; combatNotes.removeAt(i); hit = true; break; }
                else if (diff <= 65.0f) { combatFeedback = "GOOD!"; enemyHp -= 5; combatNotes.removeAt(i); hit = true; break; }
            }
            if (!hit) { combatFeedback = "MISS!"; playerHp -= 5; }
            combatFeedbackTimer = 15;
        };

        if (event->key() == Qt::Key_Space) { processHit(0); }
        else if (event->key() == Qt::Key_F) { processHit(1); }
    }
    else if (currentState == Easter) {
        if (easterEnded && event->key() == Qt::Key_Space) currentState = Festival;
    }
}

void GameWindow::keyReleaseEvent(QKeyEvent *event) {
    if (!event->isAutoRepeat()) {
        if (currentState == Farm) {
            if (event->key() == Qt::Key_W || event->key() == Qt::Key_S || event->key() == Qt::Key_A || event->key() == Qt::Key_D) player.animFrame = 0;
        } else if (currentState == Fishing) {
            if (event->key() == Qt::Key_Space) isSpaceHeld = false;
        }
    }
}

//农田劳作逻辑与随机挖出种子等设置
void GameWindow::handleFarmAction() {
    int tx = player.x; int ty = player.y;
    if (player.dir == Up) ty--; else if (player.dir == Down) ty++; else if (player.dir == Left) tx--; else if (player.dir == Right) tx++;
    if (tx < 0 || tx >= MAP_COLS || ty < 0 || ty >= MAP_ROWS) return;

    if (isInjured) { globalMsg = "你受了重伤，现在连锄头都拿不稳了！"; globalMsgTimer = 15; return; }
    if (energy < 2) { globalMsg = "过度劳累...无法挥动工具了！"; globalMsgTimer = 15; return; }
    energy -= 2;

    Tile &t = map[ty][tx];
    if (!t.isPlowed) {
        t.isPlowed = true;
        int rng = QRandomGenerator::global()->bounded(100);
        if (rng < 2) inventory.strawberrySeeds++; else if (rng < 8) inventory.sunflowerSeeds++;
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

void GameWindow::drawTextbox(QPainter &painter, int x, int y, int w, int h, QString text) {
    if (!img_textbox.isNull()) painter.drawPixmap(x, y, w, h, img_textbox);
    else { painter.setBrush(QColor(255, 240, 200)); painter.drawRect(x, y, w, h); }
    painter.setPen(QColor(60, 40, 20));
    QFont f("Comic Sans MS", 12, QFont::Bold); painter.setFont(f);
    painter.drawText(QRect(x+10, y+5, w-20, h-10), Qt::AlignCenter | Qt::TextWordWrap, text);
}

void GameWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);

    if (currentState == MainMenu) drawMainMenu(painter);
    else if (currentState == Home) { drawHome(painter); drawGlobalHUD(painter); }
    else if (currentState == Festival) { drawFestival(painter); drawGlobalHUD(painter); }
    else if (currentState == Farm) { drawFarm(painter); drawGlobalHUD(painter); }
    else if (currentState == Fishing) { drawFishing(painter); drawGlobalHUD(painter); }
    else if (currentState == Combat) { drawCombat(painter); }
    else if (currentState == Easter) { drawEaster(painter); }

    if (currentState != MainMenu && !img_exit.isNull()) {
        painter.drawPixmap(exitBtnRect, img_exit);
    }

    if (globalMsgTimer > 0) {
        drawTextbox(painter, 120, 520, 400, 60, globalMsg);
    }
}

void GameWindow::drawGlobalHUD(QPainter &painter) {
    painter.setBrush(QColor(0,0,0,150));
    painter.setPen(Qt::NoPen);
    painter.drawRect(450, 10, 180, isInjured ? 100 : 70);

    QFont f("Comic Sans MS", 14, QFont::Bold); painter.setFont(f);
    painter.setPen(QColor(255, 215, 0));
    painter.drawText(460, 35, QString("Gold: %1 G").arg(gold));
    painter.setPen(Qt::white);
    painter.drawText(460, 65, "Energy: ");

    int eWidth = (int)((float)energy / maxEnergy * 100);
    painter.setBrush(Qt::darkGray); painter.drawRect(460 + 60, 53, 100, 12);
    painter.setBrush((energy > 20) ? Qt::green : Qt::red);
    painter.drawRect(460 + 60, 53, qMax(0, eWidth), 12);

    if (isInjured) { painter.setPen(Qt::red); painter.drawText(460, 95, "Status: INJURED!!"); }
}

//主页动画设置
void GameWindow::drawMainMenu(QPainter &painter) {
    if(!img_titleBg.isNull()) painter.drawPixmap(0, 0, img_titleBg);
    else painter.fillRect(rect(), QColor(50, 80, 120));

    if (!img_bird[fastAnimFrame].isNull()) painter.drawPixmap(100, 20, img_bird[fastAnimFrame]);
    if (!img_junimo[fastAnimFrame].isNull()) painter.drawPixmap(150, 500, img_junimo[fastAnimFrame]);
    if (!img_junimo[(fastAnimFrame+2)%4].isNull()) painter.drawPixmap(450, 500, img_junimo[(fastAnimFrame+2)%4]);

    QFont f("Comic Sans MS", 22, QFont::Black); painter.setFont(f);
    QString menus[5] = {"Go Home", "Town Festival", "Farm", "River (Fishing)", "Combat"};
    for(int i=0; i<5; i++) {
        QRect btn(220, 250 + i*70, 200, 50);
        painter.setPen(Qt::black); painter.drawText(btn.adjusted(2, 2, 2, 2), Qt::AlignCenter, menus[i]);
        painter.setPen(QColor(255, 245, 220)); painter.drawText(btn, Qt::AlignCenter, menus[i]);
    }
}


//回家界面点击逻辑判断
void GameWindow::drawHome(QPainter &painter) {
    if (homeState == HomeTVShow) {
        if(!img_homeVariants[blindBoxIndex].isNull()) painter.drawPixmap(0, 0, img_homeVariants[blindBoxIndex]);
        else painter.fillRect(rect(), Qt::darkMagenta);

        QRect envRect(100, 420, 440, 200);
        if (!img_envelope.isNull()) painter.drawPixmap(envRect, img_envelope);
        else { painter.setBrush(QColor(255, 240, 200)); painter.drawRect(envRect); }
        painter.setPen(QColor(60, 40, 20)); QFont f("Comic Sans MS", 15, QFont::Bold); painter.setFont(f);
        painter.drawText(envRect.adjusted(50, 40, -50, -40), Qt::AlignCenter | Qt::TextWordWrap, homeMessages[blindBoxIndex]);

        painter.setPen(Qt::white); f.setPointSize(12); painter.setFont(f);
        painter.drawText(QRect(0, 600, 640, 30), Qt::AlignCenter, "-- 点击屏幕中心区域即可返回 --");
    } else {
        if(!img_homeBg.isNull()) painter.drawPixmap(0, 0, img_homeBg);
        else painter.fillRect(rect(), Qt::darkGray);

        if (homeState == HomeTVPrompt) {
            QRect envRect(100, 200, 440, 200);
            if (!img_envelope.isNull()) painter.drawPixmap(envRect, img_envelope);
            else { painter.setBrush(QColor(255, 240, 200)); painter.drawRect(envRect); }
            painter.setPen(QColor(60, 40, 20)); QFont f("Comic Sans MS", 15, QFont::Bold); painter.setFont(f);
            painter.drawText(envRect.adjusted(50, 40, -50, -40), Qt::AlignCenter | Qt::TextWordWrap,
                             "要花费 100 G 抽取一次房屋风格盲盒吗？\n\n[按 Y 确认][按 N 取消]");
        }
    }
}

void GameWindow::drawFestival(QPainter &painter) {
    if(!img_feastBg.isNull()) painter.drawPixmap(0, 0, img_feastBg);
    else painter.fillRect(rect(), Qt::darkGreen);

    drawTextbox(painter, 130, 60, 160, 40, "你想挑衅她？");
    drawTextbox(painter, 360, 40, 160, 40, "可爱的星星：-）");

    if (emoteState >= 1 && emoteState <= 4 && !img_emoAnim[emoteState-1].isNull()) {
        painter.drawPixmap(305, 20, img_emoAnim[emoteState-1]);
    } else if (emoteState == 5 && !img_emos[emoteId].isNull()) {
        painter.drawPixmap(305, 20, img_emos[emoteId]);
    }

    if (inShop) {
        QRect envRect(100, 150, 440, 300);
        if (!img_envelope.isNull()) painter.drawPixmap(envRect, img_envelope);
        else { painter.setBrush(QColor(255, 240, 200)); painter.drawRect(envRect); }

        painter.setPen(QColor(60, 40, 20));
        QFont f("Comic Sans MS", 16, QFont::Bold); painter.setFont(f);
        painter.drawText(QRect(100, 200, 440, 50), Qt::AlignCenter, "—— 皮埃尔的种子商店 ——");

        f.setPointSize(14); painter.setFont(f);
        painter.drawText(QRect(150, 280, 340, 30), Qt::AlignLeft, "按 1 购买 : 草莓种子 (20 G)");
        painter.drawText(QRect(150, 320, 340, 30), Qt::AlignLeft, "按 2 购买 : 向日葵种子 (10 G)");
    }
    else if (dialogueTimer > 0 && !currentDialogue.isEmpty()) {
        QRect envRect(100, 420, 440, 200);
        if (!img_envelope.isNull()) painter.drawPixmap(envRect, img_envelope);
        else { painter.setBrush(QColor(255, 240, 200)); painter.drawRect(envRect); }
        painter.setPen(QColor(60, 40, 20)); QFont f("Comic Sans MS", 15, QFont::Bold); painter.setFont(f);
        painter.drawText(envRect.adjusted(50, 40, -50, -40), Qt::AlignLeft | Qt::TextWordWrap, currentDialogue);
    }
}


//农场绘制
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

//钓鱼游戏设计
void GameWindow::drawFishing(QPainter &painter) {
    if(!img_riverBg.isNull()) painter.drawPixmap(0, 0, img_riverBg);
    else painter.fillRect(rect(), QColor(50, 150, 200));

    painter.setPen(Qt::white); QFont f("Comic Sans MS", 16, QFont::Bold); painter.setFont(f);

    if (fishState == FishIdle) painter.drawText(rect(), Qt::AlignCenter, "Press SPACE to cast line");
    else if (fishState == FishWaiting) painter.drawText(rect(), Qt::AlignCenter, "Waiting...");
    else if (fishState == FishBiting) {
        painter.setPen(Qt::red); f.setPointSize(64); painter.setFont(f); painter.drawText(rect(), Qt::AlignCenter, "!");
    } else if (fishState == FishPlaying) {
        int gameX = 500; int gameY = 150;
        painter.setBrush(QColor(0, 0, 0, 150)); painter.drawRect(gameX, gameY, 40, 300);
        painter.setBrush(QColor(0, 255, 0, 180)); painter.drawRect(gameX, gameY + barY, 40, 50);

        if (!img_fishIcon.isNull()) painter.drawPixmap(gameX + 5, gameY + fishY, img_fishIcon);
        else { painter.setBrush(Qt::red); painter.drawEllipse(gameX + 10, gameY + fishY, 20, 20); }

        painter.setBrush(Qt::transparent); painter.setPen(Qt::white); painter.drawRect(gameX + 50, gameY, 15, 300);
        painter.setBrush(Qt::green); painter.drawRect(gameX + 50, gameY + 300 - (catchProgress * 3), 15, catchProgress * 3);
    }
    else if (fishState == FishResult) {
        painter.setBrush(QColor(0,0,0,150)); painter.drawRect(rect());
        if (caughtFishIndex != -1) {
            if (!img_fishes[caughtFishIndex].isNull()) painter.drawPixmap(200, 180, img_fishes[caughtFishIndex]);
            painter.setPen(Qt::yellow); f.setPointSize(26); painter.setFont(f);
            painter.drawText(QRect(0, 120, 640, 50), Qt::AlignCenter, "CATCH!");

            if (hasTreasure) {
                int bx = 380; int by = 200;
                if (treasureState == 0 && !img_boxClosed.isNull()) painter.drawPixmap(bx, by, img_boxClosed);
                else if (treasureState >= 1 && treasureState <= 5 && !img_boxOpen[treasureState-1].isNull()) painter.drawPixmap(bx, by, img_boxOpen[treasureState-1]);
                else if (treasureState >= 6 && !img_boxOpen[4].isNull()) {
                    painter.drawPixmap(bx, by, img_boxOpen[4]);
                    QString rewText;
                    if (treasureRewardType == 0) rewText = QString("金币 +%1").arg(treasureRewardAmount);
                    else if (treasureRewardType == 1) rewText = QString("草莓种子 +%1").arg(treasureRewardAmount);
                    else rewText = QString("向日葵种子 +%1").arg(treasureRewardAmount);
                    drawTextbox(painter, bx - 60, by + 80, 180, 40, rewText);
                }
            }

            QRect envRect(100, 420, 440, 200);
            if (!img_envelope.isNull()) painter.drawPixmap(envRect, img_envelope);
            else { painter.setBrush(QColor(255, 240, 200)); painter.drawRect(envRect); }
            painter.setPen(QColor(60, 40, 20)); f.setPointSize(16); painter.setFont(f);

            QString clickHint = (hasTreasure && treasureState == 0) ? "\n(点击拆开宝箱!)" : "";
            QString customText = QString("太棒了！你钓上了一条\n【%1】 ！！%2").arg(fishNames[caughtFishIndex]).arg(clickHint);
            painter.drawText(envRect.adjusted(40, 50, -40, -50), Qt::AlignCenter | Qt::TextWordWrap, customText);

        } else {
            QRect envRect(100, 420, 440, 200);
            if (!img_envelope.isNull()) painter.drawPixmap(envRect, img_envelope);
            else { painter.setBrush(QColor(255, 240, 200)); painter.drawRect(envRect); }

            painter.setPen(QColor(60, 40, 20)); f.setPointSize(18); painter.setFont(f);
            painter.drawText(envRect, Qt::AlignCenter, "这都不行？Loser。");
        }
        f.setPointSize(12); painter.setFont(f); painter.setPen(Qt::lightGray);
        painter.drawText(QRect(0, 600, 640, 40), Qt::AlignCenter, "-- 按 空格键 或 鼠标点击 继续 --");
    }
}

//战斗背景绘制
void GameWindow::drawCombat(QPainter &painter) {
    if (!img_combatBg.isNull()) painter.drawPixmap(0, 0, img_combatBg);
    else painter.fillRect(rect(), Qt::black);

    QFont f("Comic Sans MS", 16, QFont::Bold); painter.setFont(f);

    painter.setPen(QColor(0,0,0,200)); painter.drawText(12, 42, QString("LEVEL: %1").arg(combatLevel));
    painter.setPen(Qt::white); painter.drawText(10, 40, QString("LEVEL: %1").arg(combatLevel));

    int enemyX = 480; int enemyY = 100;
    if (!img_enemy.isNull()) painter.drawPixmap(enemyX, enemyY, img_enemy);
    else { painter.setBrush(Qt::red); painter.drawRect(enemyX, enemyY, 100, 100); }

    int pX = 50; int pY = 120;
    if (!img_playerWalk[Right][0].isNull()) painter.drawPixmap(pX, pY, 64, 128, img_playerWalk[Right][0]);
    else { painter.setBrush(Qt::blue); painter.drawRect(pX, pY, 64, 128); }

    auto drawBar = [&](int x, int y, int hp, int maxH, QColor color, QString name) {
        painter.setPen(QColor(0,0,0,200)); painter.drawText(x+2, y-8, name);
        painter.setPen(Qt::white); painter.drawText(x, y - 10, name);
        painter.setBrush(Qt::darkGray); painter.drawRect(x, y, 100, 15);
        int barW = (int)((float)qMax(0, hp) / maxH * 100);
        painter.setBrush(color); painter.drawRect(x, y, barW, 15);
    };
    drawBar(50, 80, playerHp, 100, Qt::green, "Player HP");
    drawBar(480, 80, enemyHp, maxEnemyHp, Qt::red, "Enemy HP");

    int track1Y = 340;
    painter.setPen(QPen(Qt::white, 5)); painter.drawLine(0, track1Y + 16, 640, track1Y + 16);
    painter.setPen(QPen(QColor(255, 200, 100), 4)); painter.setBrush(Qt::transparent);
    painter.drawEllipse(150 - 16, track1Y, 32, 32);
    painter.setPen(Qt::black); painter.drawText(150 - 23, track1Y + 52, "[ F ]");
    painter.setPen(Qt::white); painter.drawText(150 - 25, track1Y + 50, "[ F ]");

    int track2Y = 430;
    painter.setPen(QPen(Qt::white, 5)); painter.drawLine(0, track2Y + 16, 640, track2Y + 16);
    painter.setPen(QPen(QColor(100, 200, 255), 4)); painter.setBrush(Qt::transparent);
    painter.drawEllipse(150 - 16, track2Y, 32, 32);
    painter.setPen(Qt::black); painter.drawText(150 - 43, track2Y + 52, "[SPACE]");
    painter.setPen(Qt::white); painter.drawText(150 - 45, track2Y + 50, "[SPACE]");

    for (int i = 0; i < combatNotes.size(); ++i) {
        const RhythmNote &note = combatNotes[i];
        int nY = (note.type == 1) ? track1Y : track2Y;
        if (!img_note.isNull()) painter.drawPixmap(note.x - 16, nY, img_note);
        else { painter.setBrush((note.type == 1) ? Qt::red : Qt::cyan); painter.drawEllipse(note.x - 10, nY + 6, 20, 20); }
    }

    if (combatFeedbackTimer > 0) {
        f.setPointSize(28); painter.setFont(f);
        if (combatFeedback == "PERFECT!!") painter.setPen(Qt::yellow);
        else if (combatFeedback == "MISS!") painter.setPen(Qt::red);
        else painter.setPen(Qt::white);
        painter.drawText(QRect(0, 220, 640, 50), Qt::AlignCenter, combatFeedback);
    }

    if (combatEnded) {
        painter.setBrush(QColor(0,0,0,200)); painter.drawRect(rect());
        f.setPointSize(36); painter.setFont(f);
        if (playerHp <= 0) {
            painter.setPen(Qt::red); painter.drawText(rect(), Qt::AlignCenter, "YOU DIED... INJURED");
            f.setPointSize(14); painter.setFont(f); painter.setPen(Qt::lightGray);
            painter.drawText(QRect(0, 500, 640, 50), Qt::AlignCenter, "Press SPACE to return");
        } else {
            painter.setPen(Qt::yellow); painter.drawText(rect(), Qt::AlignCenter, "VICTORY!");
            f.setPointSize(14); painter.setFont(f); painter.setPen(Qt::lightGray);
            painter.drawText(QRect(0, 500, 640, 50), Qt::AlignCenter, "Press [Y] for Next Level, [N] to Return");
        }
    }
}

//复活节彩蛋游戏设计
void GameWindow::drawEaster(QPainter &painter) {
    if(!img_easterBg.isNull()) painter.drawPixmap(0, 0, img_easterBg);
    else painter.fillRect(rect(), Qt::darkGreen);

    for (const EasterEgg &egg : easterEggs) {
        if (!egg.found) {
            if (!img_egg.isNull()) painter.drawPixmap(egg.rect.x(), egg.rect.y(), img_egg);
            else { painter.setBrush(Qt::white); painter.setPen(Qt::black); painter.drawEllipse(egg.rect); }
        }
    }

    painter.setBrush(QColor(0, 0, 0, 180)); painter.drawRect(10, 40, 220, 80);
    QFont f("Comic Sans MS", 16, QFont::Bold); painter.setFont(f); painter.setPen(Qt::white);
    painter.drawText(20, 70, QString("Found: %1 / %2").arg(easterEggsFound).arg(easterEggsTotal));

    int secondsLeft = easterTimeLeft / 5;
    if (secondsLeft <= 5) painter.setPen(Qt::red);
    painter.drawText(20, 100, QString("Time Left: %1 s").arg(secondsLeft));

    if (easterEnded) {
        painter.setBrush(QColor(0, 0, 0, 200)); painter.drawRect(rect());
        f.setPointSize(42); painter.setFont(f);
        if (easterEggsFound >= easterEggsTotal) { painter.setPen(Qt::yellow); painter.drawText(rect(), Qt::AlignCenter, "EGG HUNTER!"); }
        else { painter.setPen(Qt::red); painter.drawText(rect(), Qt::AlignCenter, "TIME'S UP!"); }
        f.setPointSize(16); painter.setFont(f); painter.setPen(Qt::lightGray);
        painter.drawText(QRect(0, 500, 640, 50), Qt::AlignCenter, "Press SPACE to return to Festival");
    }
}

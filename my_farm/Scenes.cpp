#include "Scenes.h"
#include <QRandomGenerator>

// ============== Main Menu ==============
// ============== Main Menu ==============
void SceneMainMenu::fastTick() {
    fastAnimTimer++;
    if (fastAnimTimer >= 4) { fastAnimTimer = 0; fastAnimFrame = (fastAnimFrame + 1) % 4; }

    // 改为从右向左飞（鸟头朝左，这样就是朝前飞）
    birdX -= 2.0f;
    if (birdX < -64.0f) birdX = 640.0f;
}

void SceneMainMenu::draw(QPainter &painter) {
    ResourceManager& r = ResourceManager::instance();
    if(!r.titleBg.isNull()) painter.drawPixmap(0, 0, r.titleBg); else painter.fillRect(window->rect(), QColor(50, 80, 120));

    // 【修改】去除了镜像代码，直接正常画出小鸟
    if (!r.bird[fastAnimFrame].isNull()) painter.drawPixmap((int)birdX, 20, r.bird[fastAnimFrame]);

    if (!r.junimo[fastAnimFrame].isNull()) painter.drawPixmap(150, 500, r.junimo[fastAnimFrame]);
    if (!r.junimo[(fastAnimFrame+2)%4].isNull()) painter.drawPixmap(450, 500, r.junimo[(fastAnimFrame+2)%4]);

    QFont f("Comic Sans MS", 22, QFont::Black); painter.setFont(f);
    QString menus[5] = {"Go Home", "Town Festival", "Farm", "River (Fishing)", "Combat"};
    for(int i=0; i<5; i++) {
        QRect btn(220, 250 + i*70, 200, 50);
        painter.setPen(Qt::black); painter.drawText(btn.adjusted(2, 2, 2, 2), Qt::AlignCenter, menus[i]);
        painter.setPen(QColor(255, 245, 220)); painter.drawText(btn, Qt::AlignCenter, menus[i]);
    }
}
void SceneMainMenu::mousePressEvent(QMouseEvent *event) {
    GameData& d = GameData::instance();
    int mx = event->pos().x(); int my = event->pos().y();
    if (mx >= 220 && mx <= 420) {
        if (my >= 250 && my <= 300) window->changeScene(Home);
        else if (my >= 320 && my <= 370) window->changeScene(Festival);
        else if (my >= 390 && my <= 440) window->changeScene(Farm);
        else if (my >= 460 && my <= 510) window->changeScene(Fishing);
        else if (my >= 530 && my <= 580) {
            if (d.isInjured) { d.globalMsg = "你受伤了，必须找 Harvey 治疗！"; d.globalMsgTimer = 15; }
            else if (d.energy >= 10) { d.energy -= 10; window->changeScene(Combat); }
            else { d.globalMsg = "体力不足！"; d.globalMsgTimer = 15; }
        }
    }
}

// ============== Home ==============
void SceneHome::draw(QPainter &painter) {
    ResourceManager& r = ResourceManager::instance(); GameData& d = GameData::instance();
    if (homeState == HomeTVShow) {
        if(!r.homeVariants[blindBoxIndex].isNull()) painter.drawPixmap(0, 0, r.homeVariants[blindBoxIndex]);
        QRect envRect(100, 420, 440, 200);
        if (!r.envelope.isNull()) painter.drawPixmap(envRect, r.envelope);
        painter.setPen(QColor(60, 40, 20)); QFont f("Comic Sans MS", 15, QFont::Bold); painter.setFont(f);
        painter.drawText(envRect.adjusted(50, 40, -50, -40), Qt::AlignCenter | Qt::TextWordWrap, d.homeMessages[blindBoxIndex]);
        painter.setPen(Qt::white); f.setPointSize(12); painter.setFont(f);
        painter.drawText(QRect(0, 600, 640, 30), Qt::AlignCenter, "-- 点击屏幕中心区域即可返回 --");
    } else {
        if(!r.homeBg.isNull()) painter.drawPixmap(0, 0, r.homeBg); else painter.fillRect(window->rect(), Qt::darkGray);
        if (homeState == HomeTVPrompt) {
            QRect envRect(100, 200, 440, 200);
            if (!r.envelope.isNull()) painter.drawPixmap(envRect, r.envelope);
            painter.setPen(QColor(60, 40, 20)); QFont f("Comic Sans MS", 15, QFont::Bold); painter.setFont(f);
            painter.drawText(envRect.adjusted(50, 40, -50, -40), Qt::AlignCenter | Qt::TextWordWrap, "要花费 100 G 抽取盲盒吗？\n\n[按 Y 确认][按 N 取消]");
        }
    }
}
void SceneHome::mousePressEvent(QMouseEvent *event) {
    GameData& d = GameData::instance();
    if (homeState == HomeTVShow) { if (QRect(305, 282, 28, 73).contains(event->pos())) homeState = HomeNormal; return; }
    if (QRect(21, 179, 59, 88).contains(event->pos())) {
        int earned = (d.inventory.strawberryHarvested * 20) + (d.inventory.sunflowerHarvested * 15);
        for(int i=0; i<6; i++) { earned += d.inventory.fishes[i] * 20; d.inventory.fishes[i] = 0; }
        d.inventory.strawberryHarvested = 0; d.inventory.sunflowerHarvested = 0;
        if (earned > 0) { d.gold += earned; d.globalMsg = QString("已出售获得 %1 G！").arg(earned); } else d.globalMsg = "背包空空。";
        d.globalMsgTimer = 15;
    } else if (QRect(517, 127, 103, 143).contains(event->pos())) {
        d.energy = d.maxEnergy; d.globalMsg = "体力拉满了！"; d.globalMsgTimer = 15;
    } else if (QRect(410, 335, 105, 118).contains(event->pos())) { homeState = HomeTVPrompt; }
}
void SceneHome::keyPressEvent(QKeyEvent *event) {
    GameData& d = GameData::instance();
    if (homeState == HomeTVPrompt) {
        if (event->key() == Qt::Key_Y) {
            if (d.gold >= 100) { d.gold -= 100; blindBoxIndex = QRandomGenerator::global()->bounded(6); homeState = HomeTVShow; }
            else { d.globalMsg = "金币不足！"; d.globalMsgTimer = 15; homeState = HomeNormal; }
        } else if (event->key() == Qt::Key_N) { homeState = HomeNormal; }
    }
}

// ============== Festival ==============
void SceneFestival::gameTick() { if (dialogueTimer > 0) dialogueTimer--; }
void SceneFestival::fastTick() {
    if (emoteState >= 1 && emoteState <= 4) {
        emoteTimer++; if (emoteTimer >= 3) { emoteState++; emoteTimer = 0; }
    } else if (emoteState == 5) { emoteTimer++; if (emoteTimer >= 45) { emoteState = 0; } }
}
void SceneFestival::draw(QPainter &painter) {
    ResourceManager& r = ResourceManager::instance(); GameData& d = GameData::instance();
    if(!r.feastBg.isNull()) painter.drawPixmap(0, 0, r.feastBg); else painter.fillRect(window->rect(), Qt::darkGreen);

    if (emoteState >= 1 && emoteState <= 4 && !r.emoAnim[emoteState-1].isNull()) painter.drawPixmap(305, 20, r.emoAnim[emoteState-1]);
    else if (emoteState == 5 && !r.emos[emoteId].isNull()) painter.drawPixmap(305, 20, r.emos[emoteId]);

    if (inShop) {
        QRect envRect(100, 150, 440, 300);
        if (!r.envelope.isNull()) painter.drawPixmap(envRect, r.envelope);
        painter.setPen(QColor(60, 40, 20)); QFont f("Comic Sans MS", 16, QFont::Bold); painter.setFont(f);
        painter.drawText(QRect(100, 200, 440, 50), Qt::AlignCenter, "—— 皮埃尔的种子商店 ——");
        f.setPointSize(14); painter.setFont(f);
        painter.drawText(QRect(150, 280, 340, 30), Qt::AlignLeft, "按 1 购买 : 草莓种子 (20 G)");
        painter.drawText(QRect(150, 320, 340, 30), Qt::AlignLeft, "按 2 购买 : 向日葵种子 (10 G)");
    } else if (dialogueTimer > 0 && !currentDialogue.isEmpty()) {
        QRect envRect(100, 420, 440, 200);
        if (!r.envelope.isNull()) painter.drawPixmap(envRect, r.envelope);
        painter.setPen(QColor(60, 40, 20)); QFont f("Comic Sans MS", 15, QFont::Bold); painter.setFont(f);
        painter.drawText(envRect.adjusted(50, 40, -50, -40), Qt::AlignLeft | Qt::TextWordWrap, currentDialogue);
    }
}
void SceneFestival::mousePressEvent(QMouseEvent *event) {
    if (inShop) return;
    GameData& d = GameData::instance();
    QRect emoteRect(285, 55, 72, 45);
    if (emoteRect.contains(event->pos()) && emoteState == 0) { emoteState = 1; emoteTimer = 0; emoteId = QRandomGenerator::global()->bounded(15); return; }

    for (int i = 0; i < d.npcList.size(); ++i) {
        const NPC &npc = d.npcList[i];
        if (npc.clickArea.contains(event->pos())) {
            if (d.isInjured && npc.name.contains("Harvey")) {
                harveyClickCount++;
                if (harveyClickCount >= 2) {
                    if (d.gold >= 50) {
                        d.gold -= 50; d.isInjured = false; harveyClickCount = 0;
                        d.globalMsg = "治疗成功！花费 50 G。"; d.globalMsgTimer = 15;
                        currentDialogue = npc.name + ":\n你的伤处理好了，以后不要勉强自己了。";
                    } else { currentDialogue = npc.name + ":\n抱歉，你需要支付 50 G 才能进行治疗。"; }
                } else { currentDialogue = npc.name + ":\n你受伤了？请让我来帮帮忙吧。"; }
                dialogueTimer = 25; return;
            }
            if (npc.name == "Abigail") { abigailClickCount++; if (abigailClickCount > 3) { window->changeScene(Easter); return; } }
            if (npc.name == "Pierre") { pierreClickCount++; if (pierreClickCount >= 3) { inShop = true; return; } }
            currentDialogue = npc.name + ":\n" + npc.dialogues[QRandomGenerator::global()->bounded(npc.dialogues.size())];
            dialogueTimer = 25; break;
        }
    }
}
void SceneFestival::keyPressEvent(QKeyEvent *event) {
    GameData& d = GameData::instance();
    if (inShop) {
        if (event->key() == Qt::Key_1) {
            if (d.gold >= 20) { d.gold -= 20; d.inventory.strawberrySeeds++; d.globalMsg = "购买草莓种子成功！"; d.globalMsgTimer=10; }
            else { d.globalMsg = "金币不足！"; d.globalMsgTimer=10; }
        } else if (event->key() == Qt::Key_2) {
            if (d.gold >= 10) { d.gold -= 10; d.inventory.sunflowerSeeds++; d.globalMsg = "购买向日葵种子成功！"; d.globalMsgTimer=10; }
            else { d.globalMsg = "金币不足！"; d.globalMsgTimer=10; }
        }
    }
}

// ============== Farm ==============
void SceneFarm::gameTick() { if (isHarvesting) { harvestAnimTimer--; if (harvestAnimTimer <= 0) isHarvesting = false; } }

// 【核心修复】激活猫咪AI
void SceneFarm::fastTick() { GameData::instance().myCat.update(); }

void SceneFarm::draw(QPainter &painter) {
    ResourceManager& r = ResourceManager::instance(); GameData& d = GameData::instance();
    QPixmap *currentBg = &r.bgWinter;
    if (d.currentSeason == Spring) currentBg = &r.bgSpring; else if (d.currentSeason == Summer) currentBg = &r.bgSummer; else if (d.currentSeason == Fall) currentBg = &r.bgFall;

    for (int row = 0; row < MAP_ROWS; ++row) {
        for (int c = 0; c < MAP_COLS; ++c) {
            if (!currentBg->isNull()) painter.drawPixmap(c * TILE_W, row * TILE_H, *currentBg);
        }
    }
    for (int row = 0; row < MAP_ROWS; ++row) {
        for (int c = 0; c < MAP_COLS; ++c) {
            int px = c * TILE_W; int py = row * TILE_H;
            if (d.map[row][c].isPlowed && !r.soilPlowed.isNull()) painter.drawPixmap(px, py, r.soilPlowed);
            Tile &t = d.map[row][c];
            if (t.crop != None) {
                QPixmap *cropImg = nullptr;
                if (t.crop == Strawberry) {
                    if (t.stage == Seed) cropImg = &r.strawSeed; else if (t.stage == Seedling) cropImg = &r.strawSeedling;
                    else if (t.stage == Grown) cropImg = &r.strawGrown; else if (t.stage == Fruiting) cropImg = &r.strawFruiting;
                } else if (t.crop == Sunflower) {
                    if (t.stage == Seed) cropImg = &r.sunSeed; else if (t.stage == Seedling) cropImg = &r.sunSeedling;
                    else if (t.stage == Grown) cropImg = &r.sunGrown; else if (t.stage == Fruiting) cropImg = &r.sunFruiting;
                }
                if (cropImg != nullptr && !cropImg->isNull()) painter.drawPixmap(px, py, *cropImg);
            }
        }
    }
    QPixmap *playerImg = nullptr;
    if (isHarvesting) playerImg = &r.playerHarvest; else playerImg = &r.playerWalk[d.player.dir][d.player.animFrame];
    if (playerImg != nullptr && !playerImg->isNull()) painter.drawPixmap(d.player.x * TILE_W, d.player.y * TILE_H, *playerImg);

    d.myCat.draw(painter); d.myCat.drawEmote(painter);

    painter.setBrush(QColor(0, 0, 0, 150)); painter.setPen(Qt::white); painter.drawRect(10, 30, 200, 165);
    QString seasonStr = (d.currentSeason == Winter) ? "Winter" : (d.currentSeason == Spring) ? "Spring" : (d.currentSeason == Summer) ? "Summer" : "Fall";
    painter.drawText(20, 50, "Season: " + seasonStr);
    QString seedStr = (d.player.selectedSeed == Strawberry) ? "[1] Strawberry" : "[2] Sunflower";
    painter.drawText(20, 70, "Hand: " + seedStr);
    painter.drawText(20, 90, QString("Straw. Seeds: %1").arg(d.inventory.strawberrySeeds));
    painter.drawText(20, 110, QString("Sunf. Seeds: %1").arg(d.inventory.sunflowerSeeds));
    painter.drawText(20, 130, QString("Straw. Harvests: %1").arg(d.inventory.strawberryHarvested));
    painter.drawText(20, 150, QString("Sunf. Harvests: %1").arg(d.inventory.sunflowerHarvested));
    painter.drawText(20, 170, QString("Time Left: %1s").arg((200 - d.timeTicks) / 5));
    painter.setBrush(Qt::white); painter.drawRect(10, 180, d.timeTicks, 10);
}
void SceneFarm::mousePressEvent(QMouseEvent *event) { GameData::instance().myCat.checkClick(event->pos().x(), event->pos().y()); }
void SceneFarm::keyPressEvent(QKeyEvent *event) {
    GameData& d = GameData::instance(); bool moved = false;
    if (event->key() == Qt::Key_W) { d.player.y = qMax(0, d.player.y - 1); d.player.dir = Up; moved = true; }
    if (event->key() == Qt::Key_S) { d.player.y = qMin(MAP_ROWS - 1, d.player.y + 1); d.player.dir = Down; moved = true; }
    if (event->key() == Qt::Key_A) { d.player.x = qMax(0, d.player.x - 1); d.player.dir = Left; moved = true; }
    if (event->key() == Qt::Key_D) { d.player.x = qMin(MAP_COLS - 1, d.player.x + 1); d.player.dir = Right; moved = true; }
    if (moved) d.player.animFrame = (d.player.animFrame + 1) % 4;
    if (event->key() == Qt::Key_1) d.player.selectedSeed = Strawberry;
    if (event->key() == Qt::Key_2) d.player.selectedSeed = Sunflower;
    if (event->key() == Qt::Key_J || event->key() == Qt::Key_Space) handleAction();
}
void SceneFarm::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_W || event->key() == Qt::Key_S || event->key() == Qt::Key_A || event->key() == Qt::Key_D) { GameData::instance().player.animFrame = 0; }
}
void SceneFarm::handleAction() {
    GameData& d = GameData::instance();
    int tx = d.player.x; int ty = d.player.y;
    if (d.player.dir == Up) ty--; else if (d.player.dir == Down) ty++; else if (d.player.dir == Left) tx--; else if (d.player.dir == Right) tx++;
    if (tx < 0 || tx >= MAP_COLS || ty < 0 || ty >= MAP_ROWS) return;

    if (d.isInjured) { d.globalMsg = "你受了重伤，连锄头都拿不稳了！"; d.globalMsgTimer = 15; return; }
    if (d.energy < 2) { d.globalMsg = "过度劳累...无法挥动工具！"; d.globalMsgTimer = 15; return; }
    d.energy -= 2;

    Tile &t = d.map[ty][tx];
    if (!t.isPlowed) {
        t.isPlowed = true;
        int rng = QRandomGenerator::global()->bounded(100);
        if (rng < 2) d.inventory.strawberrySeeds++; else if (rng < 8) d.inventory.sunflowerSeeds++;
    } else {
        if (t.crop != None && t.stage == Fruiting) {
            isHarvesting = true; harvestAnimTimer = 2;
            if (t.crop == Strawberry) { d.inventory.strawberryHarvested++; t.stage = Grown; }
            else if (t.crop == Sunflower) { d.inventory.sunflowerHarvested++; d.inventory.sunflowerSeeds += QRandomGenerator::global()->bounded(3); t.crop = None; t.isPlowed = false; }
        } else if (t.crop == None) {
            if (d.player.selectedSeed == Strawberry && d.inventory.strawberrySeeds > 0 && d.currentSeason == Spring) {
                d.inventory.strawberrySeeds--; t.crop = Strawberry; t.stage = Seed; t.growthTimer = 0;
            } else if (d.player.selectedSeed == Sunflower && d.inventory.sunflowerSeeds > 0 && (d.currentSeason == Summer || d.currentSeason == Fall)) {
                d.inventory.sunflowerSeeds--; t.crop = Sunflower; t.stage = Seed; t.growthTimer = 0;
            }
        }
    }
}

// ============== Fishing ==============
void SceneFishing::enter() { fishState = 0; isSpaceHeld = false; caughtFishIndex = -1; stickerTimer = 0; }
void SceneFishing::gameTick() {
    if (fishState == 1) { fishTimer--; if (fishTimer <= 0) { fishState = 2; fishTimer = 5; } }
    else if (fishState == 2) { fishTimer--; if (fishTimer <= 0) { fishState = 0; } }
}
void SceneFishing::fastTick() {
    if (stickerTimer > 0) stickerTimer--;
    if (fishState == 3) {
        if (isSpaceHeld) barVy -= 2.0f; else barVy += 1.5f;
        barVy *= 0.85f; barY += barVy;
        if (barY < 0) { barY = 0; barVy = 0; } if (barY > 300 - 50) { barY = 300 - 50; barVy = 0; }
        auto rng = QRandomGenerator::global();
        if (std::abs(fishY - fishTarget) < 5 || rng->bounded(100) < 5) fishTarget = rng->bounded(250);
        if (fishY < fishTarget) fishY += 3.0f; else fishY -= 3.0f;
        if (fishY >= barY && fishY <= barY + 50) catchProgress += 2.0f; else catchProgress -= 1.0f;

        if (catchProgress >= 100) {
            fishState = 4; caughtFishIndex = rng->bounded(6);
            GameData::instance().inventory.fishes[caughtFishIndex]++;
            if (rng->bounded(100) < 50) {
                hasTreasure = true; treasureState = 0; treasureRewardType = rng->bounded(3);
                treasureRewardAmount = (treasureRewardType == 0) ? rng->bounded(50, 150) : rng->bounded(1, 4);
            } else hasTreasure = false;
        }
        if (catchProgress <= 0) { fishState = 4; caughtFishIndex = -1; hasTreasure = false; }
    }
    if (fishState == 4 && hasTreasure && treasureState >= 1 && treasureState <= 5) {
        treasureTimer++;
        if (treasureTimer > 3) {
            treasureState++; treasureTimer = 0;
            if (treasureState == 6) {
                GameData& d = GameData::instance();
                if (treasureRewardType == 0) d.gold += treasureRewardAmount;
                else if (treasureRewardType == 1) d.inventory.strawberrySeeds += treasureRewardAmount;
                else d.inventory.sunflowerSeeds += treasureRewardAmount;
            }
        }
    }
}
void SceneFishing::draw(QPainter &painter) {
    ResourceManager& r = ResourceManager::instance(); GameData& d = GameData::instance();
    if(!r.riverBg.isNull()) painter.drawPixmap(0, 0, r.riverBg); else painter.fillRect(window->rect(), QColor(50, 150, 200));

    painter.setPen(Qt::white); QFont f("Comic Sans MS", 16, QFont::Bold); painter.setFont(f);

    // 【核心修复】画出华丽的上钩贴纸！
    if (stickerTimer > 0) {
        if (!r.sticker.isNull()) painter.drawPixmap(320 - r.sticker.width()/2, 180, r.sticker);
        else { painter.setPen(Qt::yellow); f.setPointSize(32); painter.setFont(f); painter.drawText(QRect(0, 180, 640, 50), Qt::AlignCenter, "上钩了!"); }
    }

    if (fishState == 0) painter.drawText(window->rect(), Qt::AlignCenter, "Press SPACE to cast line");
    else if (fishState == 1) painter.drawText(window->rect(), Qt::AlignCenter, "Waiting...");
    else if (fishState == 2) { painter.setPen(Qt::red); f.setPointSize(64); painter.setFont(f); painter.drawText(window->rect(), Qt::AlignCenter, "!"); }
    else if (fishState == 3) {
        int gameX = 500; int gameY = 150;
        painter.setBrush(QColor(0, 0, 0, 150)); painter.drawRect(gameX, gameY, 40, 300);
        painter.setBrush(QColor(0, 255, 0, 180)); painter.drawRect(gameX, gameY + barY, 40, 50);

        // 【核心修复】如果没加载出鱼图标，必须画一个红球，防止它消失！
        if (!r.fishIcon.isNull()) painter.drawPixmap(gameX + 5, gameY + fishY, r.fishIcon);
        else { painter.setBrush(Qt::red); painter.drawEllipse(gameX + 10, gameY + fishY, 20, 20); }

        painter.setBrush(Qt::transparent); painter.setPen(Qt::white); painter.drawRect(gameX + 50, gameY, 15, 300);
        painter.setBrush(Qt::green); painter.drawRect(gameX + 50, gameY + 300 - (catchProgress * 3), 15, catchProgress * 3);
    } else if (fishState == 4) {
        painter.setBrush(QColor(0,0,0,150)); painter.drawRect(window->rect());
        if (caughtFishIndex != -1) {
            if (!r.fishes[caughtFishIndex].isNull()) painter.drawPixmap(200, 180, r.fishes[caughtFishIndex]);
            painter.setPen(Qt::yellow); f.setPointSize(26); painter.setFont(f); painter.drawText(QRect(0, 120, 640, 50), Qt::AlignCenter, "CATCH!");

            if (hasTreasure) {
                int bx = 380; int by = 200;
                if (treasureState == 0 && !r.boxClosed.isNull()) painter.drawPixmap(bx, by, r.boxClosed);
                else if (treasureState >= 1 && treasureState <= 5 && !r.boxOpen[treasureState-1].isNull()) painter.drawPixmap(bx, by, r.boxOpen[treasureState-1]);
                else if (treasureState >= 6 && !r.boxOpen[4].isNull()) {
                    painter.drawPixmap(bx, by, r.boxOpen[4]);
                    QString rewText = (treasureRewardType == 0) ? QString("金币 +%1").arg(treasureRewardAmount) :
                                          (treasureRewardType == 1) ? QString("草莓种子 +%1").arg(treasureRewardAmount) : QString("向日葵种子 +%1").arg(treasureRewardAmount);
                    painter.setPen(Qt::white); f.setPointSize(12); painter.setFont(f);
                    painter.drawText(QRect(bx - 60, by + 80, 180, 40), Qt::AlignCenter, rewText);
                }
            }
            QRect envRect(100, 420, 440, 200);
            if (!r.envelope.isNull()) painter.drawPixmap(envRect, r.envelope);
            painter.setPen(QColor(60, 40, 20)); f.setPointSize(16); painter.setFont(f);
            QString clickHint = (hasTreasure && treasureState == 0) ? "\n(点击空格拆开宝箱!)" : "";
            painter.drawText(envRect.adjusted(40, 50, -40, -50), Qt::AlignCenter | Qt::TextWordWrap, QString("太棒了！钓上了一条\n【%1】！%2").arg(d.fishNames[caughtFishIndex]).arg(clickHint));
        } else {
            QRect envRect(100, 420, 440, 200);
            if (!r.envelope.isNull()) painter.drawPixmap(envRect, r.envelope);
            painter.setPen(QColor(60, 40, 20)); f.setPointSize(18); painter.setFont(f);
            painter.drawText(envRect, Qt::AlignCenter, "这都不行？Loser。");
        }
        f.setPointSize(12); painter.setPen(Qt::lightGray); painter.setFont(f); painter.drawText(QRect(0, 600, 640, 40), Qt::AlignCenter, "-- 按 空格键 继续 --");
    }
}
void SceneFishing::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Space) {
        GameData& d = GameData::instance();
        isSpaceHeld = true;
        if (fishState == 0) {
            if (d.isInjured) { d.globalMsg = "你受伤了，必须找 Harvey 治疗！"; d.globalMsgTimer = 15; return; }
            if (d.energy >= 5) {
                d.energy -= 5; fishState = 1; fishTimer = QRandomGenerator::global()->bounded(10, 25);
            } else { d.globalMsg = "体力不够了..."; d.globalMsgTimer = 15; }
        } else if (fishState == 2) {
            fishState = 3; barY = 150; barVy = 0; fishY = 150; fishTarget = 150; catchProgress = 30;
            stickerTimer = 30; // 触发上钩贴纸
        } else if (fishState == 4) {
            if (hasTreasure && treasureState == 0) treasureState = 1;
            else if (!hasTreasure || treasureState >= 6) fishState = 0;
        }
    }
}
void SceneFishing::keyReleaseEvent(QKeyEvent *event) { if (event->key() == Qt::Key_Space) isSpaceHeld = false; }

// ============== Combat ==============
void SceneCombat::enter() { initLevel(false); }
void SceneCombat::initLevel(bool nextLevel) {
    playerHp = 100; if (!nextLevel) level = 1;
    maxEnemyHp = 100 + (level - 1) * 35; enemyHp = maxEnemyHp;
    notes.clear(); spawnTimer = 30; feedback = QString("LEVEL %1 READY!").arg(level);
    fbTimer = 30; ended = false;
}
void SceneCombat::fastTick() {
    if (ended) return;
    if (fbTimer > 0) fbTimer--;

    float speed = 12.0f + level * 3.5f;
    for (int i = 0; i < notes.size(); ++i) notes[i].x -= speed;

    for (int i = notes.size() - 1; i >= 0; --i) {
        if (notes[i].x < 50) { notes.removeAt(i); playerHp -= 10; feedback = "MISS!"; fbTimer = 20; }
    }

    spawnTimer--;
    if (spawnTimer <= 0) {
        auto rng = QRandomGenerator::global();
        int doubleChance = qMin(60, 10 + level * 8);
        int rngVal = rng->bounded(100);
        if (rngVal < doubleChance) { notes.append(RhythmNote{ 640.0f, 0 }); notes.append(RhythmNote{ 640.0f, 1 }); }
        else if (rngVal < 50 + doubleChance / 2) { notes.append(RhythmNote{ 640.0f, 0 }); }
        else { notes.append(RhythmNote{ 640.0f, 1 }); }
        spawnTimer = rng->bounded(qMax(5, 20 - level * 2), qMax(12, 35 - level * 3));
    }

    GameData& d = GameData::instance();
    if (playerHp <= 0) {
        playerHp = 0; ended = true; d.isInjured = true;
        d.globalMsg = "你受了重伤！必须找 Harvey 治疗！"; d.globalMsgTimer = 30;
    }
    if (enemyHp <= 0 && !ended) {
        enemyHp = 0; ended = true;
        int reward = 10 + level * 10; d.gold += reward;
        d.globalMsg = QString("战斗胜利！获得 %1 G").arg(reward); d.globalMsgTimer = 15;
    }
}
void SceneCombat::draw(QPainter &painter) {
    ResourceManager& r = ResourceManager::instance();
    if (!r.combatBg.isNull()) painter.drawPixmap(0, 0, r.combatBg); else painter.fillRect(window->rect(), Qt::black);

    QFont f("Comic Sans MS", 16, QFont::Bold); painter.setFont(f);
    painter.setPen(QColor(0,0,0,200)); painter.drawText(12, 42, QString("LEVEL: %1").arg(level));
    painter.setPen(Qt::white); painter.drawText(10, 40, QString("LEVEL: %1").arg(level));

    if (!r.enemy.isNull()) painter.drawPixmap(480, 100, r.enemy);
    if (!r.playerWalk[Right][0].isNull()) painter.drawPixmap(50, 120, 64, 128, r.playerWalk[Right][0]);

    auto drawBar = [&](int x, int y, int hp, int maxH, QColor color, QString name) {
        painter.setPen(QColor(0,0,0,200)); painter.drawText(x+2, y-8, name); painter.setPen(Qt::white); painter.drawText(x, y - 10, name);
        painter.setBrush(Qt::darkGray); painter.drawRect(x, y, 100, 15);
        painter.setBrush(color); painter.drawRect(x, y, (int)((float)qMax(0, hp) / maxH * 100), 15);
    };
    drawBar(50, 80, playerHp, 100, Qt::green, "Player HP"); drawBar(480, 80, enemyHp, maxEnemyHp, Qt::red, "Enemy HP");

    int track1Y = 340; painter.setPen(QPen(Qt::white, 5)); painter.drawLine(0, track1Y + 16, 640, track1Y + 16);
    painter.setPen(QPen(QColor(255, 200, 100), 4)); painter.setBrush(Qt::transparent); painter.drawEllipse(150 - 16, track1Y, 32, 32);
    painter.setPen(Qt::black); painter.drawText(150 - 23, track1Y + 52, "[ F ]"); painter.setPen(Qt::white); painter.drawText(150 - 25, track1Y + 50, "[ F ]");

    int track2Y = 430; painter.setPen(QPen(Qt::white, 5)); painter.drawLine(0, track2Y + 16, 640, track2Y + 16);
    painter.setPen(QPen(QColor(100, 200, 255), 4)); painter.setBrush(Qt::transparent); painter.drawEllipse(150 - 16, track2Y, 32, 32);
    painter.setPen(Qt::black); painter.drawText(150 - 43, track2Y + 52, "[SPACE]"); painter.setPen(Qt::white); painter.drawText(150 - 45, track2Y + 50, "[SPACE]");

    for (const RhythmNote &note : notes) {
        int nY = (note.type == 1) ? track1Y : track2Y;
        if (!r.note.isNull()) painter.drawPixmap(note.x - 16, nY, r.note);
    }

    if (fbTimer > 0) {
        f.setPointSize(28); painter.setFont(f);
        if (feedback == "PERFECT!!") painter.setPen(Qt::yellow); else if (feedback == "MISS!") painter.setPen(Qt::red); else painter.setPen(Qt::white);
        painter.drawText(QRect(0, 220, 640, 50), Qt::AlignCenter, feedback);
    }

    if (ended) {
        painter.setBrush(QColor(0,0,0,200)); painter.drawRect(window->rect()); f.setPointSize(36); painter.setFont(f);
        if (playerHp <= 0) {
            painter.setPen(Qt::red); painter.drawText(window->rect(), Qt::AlignCenter, "YOU DIED... INJURED");
            f.setPointSize(14); painter.setFont(f); painter.setPen(Qt::lightGray); painter.drawText(QRect(0, 500, 640, 50), Qt::AlignCenter, "Press SPACE to return");
        } else {
            painter.setPen(Qt::yellow); painter.drawText(window->rect(), Qt::AlignCenter, "VICTORY!");
            f.setPointSize(14); painter.setFont(f); painter.setPen(Qt::lightGray); painter.drawText(QRect(0, 500, 640, 50), Qt::AlignCenter, "Press [Y] Next Level, [N] Return");
        }
    }
}
void SceneCombat::keyPressEvent(QKeyEvent *event) {
    if (ended) {
        if (enemyHp <= 0) {
            if (event->key() == Qt::Key_Y) { level++; initLevel(true); } else if (event->key() == Qt::Key_N) { window->changeScene(MainMenu); }
        } else { if (event->key() == Qt::Key_Space) window->changeScene(MainMenu); }
        return;
    }
    auto processHit = [&](int targetType) {
        bool hit = false;
        for (int i = 0; i < notes.size(); ++i) {
            if (notes[i].type != targetType) continue;
            float diff = std::abs(notes[i].x - 150.0f);
            if (diff <= 30.0f) { feedback = "PERFECT!!"; enemyHp -= 15; notes.removeAt(i); hit = true; break; }
            else if (diff <= 65.0f) { feedback = "GOOD!"; enemyHp -= 5; notes.removeAt(i); hit = true; break; }
        }
        if (!hit) { feedback = "MISS!"; playerHp -= 5; }
        fbTimer = 15;
    };
    if (event->key() == Qt::Key_Space) processHit(0); else if (event->key() == Qt::Key_F) processHit(1);
}

// ============== Easter ==============
void SceneEaster::enter() {
    found = 0; timeLeft = 75; ended = false; eggs.clear();
    auto rng = QRandomGenerator::global();
    for (int i = 0; i < total; ++i) eggs.append({QRect(rng->bounded(20, 600), rng->bounded(120, 600), 12, 12), false});
}
void SceneEaster::gameTick() {
    if (ended) return;
    timeLeft--; if (timeLeft <= 0) { timeLeft = 0; ended = true; }
    if (found >= total) ended = true;
}
void SceneEaster::draw(QPainter &painter) {
    ResourceManager& r = ResourceManager::instance();
    if(!r.easterBg.isNull()) painter.drawPixmap(0, 0, r.easterBg); else painter.fillRect(window->rect(), Qt::darkGreen);

    for (const EasterEgg &egg : eggs) {
        if (!egg.found) {
            if (!r.egg.isNull()) painter.drawPixmap(egg.rect.x(), egg.rect.y(), r.egg);
        }
    }

    painter.setBrush(QColor(0, 0, 0, 180)); painter.drawRect(10, 40, 220, 80);
    QFont f("Comic Sans MS", 16, QFont::Bold); painter.setFont(f); painter.setPen(Qt::white);
    painter.drawText(20, 70, QString("Found: %1 / %2").arg(found).arg(total));
    int secondsLeft = timeLeft / 5; if (secondsLeft <= 5) painter.setPen(Qt::red);
    painter.drawText(20, 100, QString("Time Left: %1 s").arg(secondsLeft));

    if (ended) {
        painter.setBrush(QColor(0, 0, 0, 200)); painter.drawRect(window->rect()); f.setPointSize(42); painter.setFont(f);
        if (found >= total) { painter.setPen(Qt::yellow); painter.drawText(window->rect(), Qt::AlignCenter, "EGG HUNTER!"); }
        else { painter.setPen(Qt::red); painter.drawText(window->rect(), Qt::AlignCenter, "TIME'S UP!"); }
        f.setPointSize(16); painter.setFont(f); painter.setPen(Qt::lightGray);
        painter.drawText(QRect(0, 500, 640, 50), Qt::AlignCenter, "Press SPACE to return to Festival");
    }
}
void SceneEaster::mousePressEvent(QMouseEvent *event) {
    if (!ended) {
        for (int i = 0; i < eggs.size(); ++i) {
            if (!eggs[i].found && eggs[i].rect.contains(event->pos())) { eggs[i].found = true; found++; break; }
        }
    }
}
void SceneEaster::keyPressEvent(QKeyEvent *event) {
    if (ended && event->key() == Qt::Key_Space) window->changeScene(Festival);
}

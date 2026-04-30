#include "GameWindow.h"
#include "Scenes.h"
#include <QPainter>

GameWindow::GameWindow(QWidget *parent) : QWidget(parent), currentState(MainMenu) {
    setFixedSize(640, 640);
    setWindowTitle("星露谷 (纯净OOP工业版)");

    exitBtnRect = QRect(15, 570, 64, 54);

    ResourceManager::instance().loadAll();
    GameData::instance();

    scenes[MainMenu] = new SceneMainMenu(this);
    scenes[Home] = new SceneHome(this);
    scenes[Festival] = new SceneFestival(this);
    scenes[Farm] = new SceneFarm(this);
    scenes[Fishing] = new SceneFishing(this);
    scenes[Combat] = new SceneCombat(this);
    scenes[Easter] = new SceneEaster(this);

    timer = new QTimer(this); connect(timer, &QTimer::timeout, this, &GameWindow::gameTick); timer->start(200);
    fastTimer = new QTimer(this); connect(fastTimer, &QTimer::timeout, this, &GameWindow::fastTick); fastTimer->start(33);
}

GameWindow::~GameWindow() {
    for (int i = 0; i < 7; ++i) delete scenes[i];
}

void GameWindow::changeScene(int stateIndex) {
    if (stateIndex >= 0 && stateIndex < 7) {
        currentState = stateIndex;
        scenes[currentState]->enter();
    }
}

void GameWindow::gameTick() {
    GameData& d = GameData::instance();
    d.timeTicks++;

    for (int r = 0; r < MAP_ROWS; ++r) {
        for (int c = 0; c < MAP_COLS; ++c) {
            Tile &t = d.map[r][c];
            if (t.crop != None && t.stage != Fruiting) {
                bool canGrow = false;
                if (t.crop == Strawberry && d.currentSeason == Spring) canGrow = true;
                if (t.crop == Sunflower && (d.currentSeason == Summer || d.currentSeason == Fall)) canGrow = true;
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

    if (d.timeTicks >= 200) {
        d.timeTicks = 0;
        int next = d.currentSeason + 1;
        if (next > Fall) d.currentSeason = Winter; else d.currentSeason = static_cast<Season>(next);
        for (int r=0; r<MAP_ROWS; ++r) for (int c=0; c<MAP_COLS; ++c) {
                if (d.map[r][c].crop == Strawberry && d.currentSeason != Spring) d.map[r][c].crop = None;
                if (d.map[r][c].crop == Sunflower && d.currentSeason == Winter) d.map[r][c].crop = None;
            }
    }

    if (d.globalMsgTimer > 0) d.globalMsgTimer--;
    scenes[currentState]->gameTick();
}

void GameWindow::fastTick() {
    scenes[currentState]->fastTick();
    update();
}

void GameWindow::mousePressEvent(QMouseEvent *event) {
    if (currentState != MainMenu && exitBtnRect.contains(event->pos())) {
        changeScene(MainMenu); return;
    }
    scenes[currentState]->mousePressEvent(event);
}

void GameWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape && currentState != MainMenu) { changeScene(MainMenu); return; }
    scenes[currentState]->keyPressEvent(event);
}

void GameWindow::keyReleaseEvent(QKeyEvent *event) {
    // 【核心修复】拦截长按键盘连发！解决钓鱼按不住空格的问题！
    if (event->isAutoRepeat()) return;
    scenes[currentState]->keyReleaseEvent(event);
}

void GameWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event); QPainter painter(this);

    scenes[currentState]->draw(painter);

    if (currentState != MainMenu && !ResourceManager::instance().exitBtn.isNull()) {
        painter.drawPixmap(exitBtnRect, ResourceManager::instance().exitBtn);
    }
    if (currentState != MainMenu) drawGlobalHUD(painter);

    if (GameData::instance().globalMsgTimer > 0) {
        drawTextbox(painter, 120, 520, 400, 60, GameData::instance().globalMsg);
    }
}

void GameWindow::drawTextbox(QPainter &painter, int x, int y, int w, int h, QString text) {
    if (!ResourceManager::instance().textbox.isNull()) painter.drawPixmap(x, y, w, h, ResourceManager::instance().textbox);
    else { painter.setBrush(QColor(255, 240, 200)); painter.drawRect(x, y, w, h); }
    painter.setPen(QColor(60, 40, 20)); QFont f("Comic Sans MS", 12, QFont::Bold); painter.setFont(f);
    painter.drawText(QRect(x+10, y+5, w-20, h-10), Qt::AlignCenter | Qt::TextWordWrap, text);
}

void GameWindow::drawGlobalHUD(QPainter &painter) {
    GameData& d = GameData::instance();
    painter.setBrush(QColor(0,0,0,150)); painter.setPen(Qt::NoPen);
    painter.drawRect(450, 10, 180, d.isInjured ? 100 : 70);

    QFont f("Comic Sans MS", 14, QFont::Bold); painter.setFont(f);
    painter.setPen(QColor(255, 215, 0)); painter.drawText(460, 35, QString("Gold: %1 G").arg(d.gold));
    painter.setPen(Qt::white); painter.drawText(460, 65, "Energy: ");

    int eWidth = (int)((float)d.energy / d.maxEnergy * 100);
    painter.setBrush(Qt::darkGray); painter.drawRect(460 + 60, 53, 100, 12);
    painter.setBrush((d.energy > 20) ? Qt::green : Qt::red); painter.drawRect(460 + 60, 53, qMax(0, eWidth), 12);
    if (d.isInjured) { painter.setPen(Qt::red); painter.drawText(460, 95, "Status: INJURED!!"); }
}

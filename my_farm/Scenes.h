#ifndef SCENES_H
#define SCENES_H

#include "IScene.h"
#include "GameData.h"
#include "ResourceManager.h"
#include "GameWindow.h"

class SceneMainMenu : public IScene {
    float birdX = 640.0f;
    int fastAnimTimer = 0, fastAnimFrame = 0;
public:
    SceneMainMenu(GameWindow* w) : IScene(w) {}
    void fastTick() override;
    void draw(QPainter &painter) override;
    void mousePressEvent(QMouseEvent *event) override;
};

class SceneHome : public IScene {
    HomeState homeState = HomeNormal;
    int blindBoxIndex = 0;
public:
    SceneHome(GameWindow* w) : IScene(w) {}
    void draw(QPainter &painter) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
};

class SceneFestival : public IScene {
    int emoteState = 0, emoteTimer = 0, emoteId = 0;
    bool inShop = false;
    int abigailClickCount = 0, pierreClickCount = 0, harveyClickCount = 0;
    QString currentDialogue;
    int dialogueTimer = 0;
public:
    SceneFestival(GameWindow* w) : IScene(w) {}
    void fastTick() override;
    void gameTick() override;
    void draw(QPainter &painter) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
};

class SceneFarm : public IScene {
    bool isHarvesting = false;
    int harvestAnimTimer = 0;
    void handleAction();
public:
    SceneFarm(GameWindow* w) : IScene(w) {}
    void gameTick() override;
    void fastTick() override;
    void draw(QPainter &painter) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
};

class SceneFishing : public IScene {
    int fishState = 0; // 0:Idle, 1:Wait, 2:Bite, 3:Play, 4:Result
    int fishTimer = 0, stickerTimer = 0;
    float barY=0, barVy=0, fishY=0, fishTarget=0, catchProgress=0;
    bool isSpaceHeld = false, hasTreasure = false;
    int treasureState=0, treasureTimer=0, treasureRewardType=0, treasureRewardAmount=0;
    int caughtFishIndex = -1;
public:
    SceneFishing(GameWindow* w) : IScene(w) {}
    void enter() override; // 每次进入重置状态
    void gameTick() override;
    void fastTick() override;
    void draw(QPainter &painter) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
};

class SceneCombat : public IScene {
    QList<RhythmNote> notes;
    int playerHp = 100, enemyHp = 100, maxEnemyHp = 100;
    int spawnTimer = 30, fbTimer = 0, level = 1;
    QString feedback = "READY!";
    bool ended = false;
public:
    SceneCombat(GameWindow* w) : IScene(w) {}
    void enter() override;
    void initLevel(bool nextLevel);
    void fastTick() override;
    void draw(QPainter &painter) override;
    void keyPressEvent(QKeyEvent *event) override;
};

class SceneEaster : public IScene {
    QList<EasterEgg> eggs;
    int timeLeft = 75, found = 0, total = 12;
    bool ended = false;
public:
    SceneEaster(GameWindow* w) : IScene(w) {}
    void enter() override;
    void gameTick() override;
    void draw(QPainter &painter) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
};

#endif
//游戏所有场景类的头文件，实现了7个具体场景（主菜单、家园、节日、农场、钓鱼、战斗、彩蛋），每个场景都继承自IScene接口并实现各自的游戏逻辑、更新和绘制方法

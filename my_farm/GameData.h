#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <QString>
#include <QList>      // <--- 核心修复：之前漏掉了这个容器头文件！
#include "GlobalConfig.h"
#include "FarmCat.h"

class GameData {
public:
    static GameData& instance(); // 单例入口

    int gold;
    int energy, maxEnergy;
    QString globalMsg;
    int globalMsgTimer;
    bool isInjured;

    Season currentSeason;
    int timeTicks;

    Tile map[MAP_ROWS][MAP_COLS];
    Player player;
    Inventory inventory;
    FarmCat myCat;

    QString fishNames[6];
    QString homeMessages[6];
    QList<NPC> npcList;

private:
    GameData();
    void initNPCs();
};

#endif // GAMEDATA_H

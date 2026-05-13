#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <QString>
#include <QList>
#include "GlobalConfig.h"
#include "FarmCat.h"

class GameData {
public:
    static GameData& instance();

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

#endif
//单例定义了全局游戏数据管理中心，集中存储玩家属性、地图、背包、npc以及当前游戏季节，玩家状态之类的所有游戏状态数据

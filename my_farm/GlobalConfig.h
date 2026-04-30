#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H

#include <QString>
#include <QRect>
#include <QStringList>

enum GameState { MainMenu, Home, Festival, Farm, Fishing, Combat, Easter };
enum Season { Winter, Spring, Summer, Fall };
enum CropType { None, Strawberry, Sunflower };
enum CropStage { Seed, Seedling, Grown, Fruiting };
enum Direction { Up = 0, Down = 1, Left = 2, Right = 3 };

const int TILE_W = 32;
const int TILE_H = 64;
const int MAP_COLS = 20;
const int MAP_ROWS = 10;

struct Tile { bool isPlowed = false; CropType crop = None; CropStage stage = Seed; int growthTimer = 0; };
struct Inventory {
    int strawberrySeeds = 0; int sunflowerSeeds = 0;
    int strawberryHarvested = 0; int sunflowerHarvested = 0;
    int fishes[6] = {0, 0, 0, 0, 0, 0};
};
struct Player { int x = 5; int y = 5; Direction dir = Down; CropType selectedSeed = Strawberry; int animFrame = 0; };

struct NPC { QString name; QRect clickArea; QStringList dialogues; };
struct RhythmNote { float x; int type; };
struct EasterEgg { QRect rect; bool found; };

enum HomeState { HomeNormal, HomeTVPrompt, HomeTVShow };

#endif
//全局配置头文件，定义了所有枚举类型、参数和核心数据结构

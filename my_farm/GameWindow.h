#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>
#include <QTimer>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPixmap>
#include <QStringList>
#include <QRect>
#include <QList>

enum GameState { MainMenu, Home, Festival, Farm, Fishing };
// 【修改】去掉了 NextWinter，四季将无限循环
enum Season { Winter, Spring, Summer, Fall };
enum CropType { None, Strawberry, Sunflower };
enum CropStage { Seed, Seedling, Grown, Fruiting };
enum Direction { Up = 0, Down = 1, Left = 2, Right = 3 };

struct Tile { bool isPlowed = false; CropType crop = None; CropStage stage = Seed; int growthTimer = 0; };
struct Inventory { int strawberrySeeds = 0; int sunflowerSeeds = 0; int strawberryHarvested = 0; int sunflowerHarvested = 0; };
struct Player { int x = 5; int y = 5; Direction dir = Down; CropType selectedSeed = Strawberry; int animFrame = 0; };

struct NPC {
    QString name;
    QRect clickArea;
    QStringList dialogues;
};

class GameWindow : public QWidget {
    Q_OBJECT

public:
    GameWindow(QWidget *parent = nullptr);
    ~GameWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void gameTick();

private:
    void handleFarmAction();
    void updateCrops();
    void updateFishing();
    void initNPCs(); // 【新增】专门用来初始化所有人物坐标和对话的函数

    void drawMainMenu(QPainter &painter);
    void drawHome(QPainter &painter);
    void drawFestival(QPainter &painter);
    void drawFarm(QPainter &painter);
    void drawFishing(QPainter &painter);

    static const int TILE_W = 32;
    static const int TILE_H = 64;
    static const int MAP_COLS = 20;
    static const int MAP_ROWS = 10;

    GameState currentState;

    Tile map[MAP_ROWS][MAP_COLS];
    Player player;
    Inventory inventory;
    Season currentSeason;
    int timeTicks;

    bool isHarvesting;
    int harvestAnimTimer;

    QList<NPC> npcList;
    QString currentDialogue;
    int dialogueTimer;

    enum FishState { FishIdle, FishWaiting, FishBiting, FishPlaying, FishResult };
    FishState fishState;
    int fishTimer;
    float barY, barVy;
    float fishY, fishTarget;
    float catchProgress;
    bool isSpaceHeld;

    // 图片资源
    QPixmap img_bgWinter, img_bgSpring, img_bgSummer, img_bgFall;
    QPixmap img_soilPlowed, img_playerHarvest;
    QPixmap img_strawSeed, img_strawSeedling, img_strawGrown, img_strawFruiting;
    QPixmap img_sunSeed, img_sunSeedling, img_sunGrown, img_sunFruiting;
    QPixmap img_playerWalk[4][4];

    QPixmap img_titleBg; // 【新增】主菜单背景
    QPixmap img_homeBg;
    QPixmap img_feastBg;
    QPixmap img_riverBg;
    QPixmap img_fishIcon;

    QPixmap img_fishes[6];
    int caughtFishIndex;
    QString fishNames[6];

    QTimer *timer;
};

#endif // GAMEWINDOW_H

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

enum GameState { MainMenu, Home, Festival, Farm, Fishing, Combat, Easter };
enum Season { Winter, Spring, Summer, Fall };
enum CropType { None, Strawberry, Sunflower };
enum CropStage { Seed, Seedling, Grown, Fruiting };
enum Direction { Up = 0, Down = 1, Left = 2, Right = 3 };

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
    void fastTick();

private:
    void handleFarmAction();
    void updateCrops();
    void updateFishing();
    void initNPCs();
    void initCombat(bool nextLevel = false);
    void updateCombat();
    void initEasterGame();
    void updateEaster();

    void drawMainMenu(QPainter &painter);
    void drawHome(QPainter &painter);
    void drawFestival(QPainter &painter);
    void drawFarm(QPainter &painter);
    void drawFishing(QPainter &painter);
    void drawCombat(QPainter &painter);
    void drawEaster(QPainter &painter);
    void drawGlobalHUD(QPainter &painter);
    void drawTextbox(QPainter &painter, int x, int y, int w, int h, QString text); // 绘制窄边框文本

    static const int TILE_W = 32;
    static const int TILE_H = 64;
    static const int MAP_COLS = 20;
    static const int MAP_ROWS = 10;

    GameState currentState;

    int gold;
    int energy;
    int maxEnergy;
    QString globalMsg;
    int globalMsgTimer;

    bool isInjured;
    int harveyClickCount;

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

    int abigailClickCount;
    int pierreClickCount;
    bool inShop;

    // 钓鱼与宝箱
    enum FishState { FishIdle, FishWaiting, FishBiting, FishPlaying, FishResult };
    FishState fishState;
    int fishTimer;
    float barY, barVy;
    float fishY, fishTarget;
    float catchProgress;
    bool isSpaceHeld;
    bool hasTreasure;
    int treasureState; // 0=关, 1~5=开启动画, 6=开启完毕
    int treasureTimer;
    int treasureRewardType;
    int treasureRewardAmount;

    QList<RhythmNote> combatNotes;
    int playerHp;
    int enemyHp;
    int maxEnemyHp;
    int combatSpawnTimer;
    QString combatFeedback;
    int combatFeedbackTimer;
    bool combatEnded;
    int combatLevel;

    QList<EasterEgg> easterEggs;
    int easterTimeLeft;
    int easterEggsFound;
    int easterEggsTotal;
    bool easterEnded;

    HomeState homeState;
    int blindBoxIndex;
    QString homeMessages[6];

    // 动画通用计数器
    int fastAnimTimer;
    int fastAnimFrame;

    // 表情彩蛋
    int emoteState; // 0=关, 1~4=冒出动画, 5=显示表情
    int emoteTimer;
    int emoteId;

    // 图片资源
    QPixmap img_bgWinter, img_bgSpring, img_bgSummer, img_bgFall;
    QPixmap img_soilPlowed, img_playerHarvest;
    QPixmap img_strawSeed, img_strawSeedling, img_strawGrown, img_strawFruiting;
    QPixmap img_sunSeed, img_sunSeedling, img_sunGrown, img_sunFruiting;
    QPixmap img_playerWalk[4][4];

    QPixmap img_titleBg, img_homeBg, img_feastBg, img_riverBg;
    QPixmap img_fishIcon, img_envelope;
    QPixmap img_enemy, img_note, img_easterBg, img_egg;

    QPixmap img_homeVariants[6];
    QPixmap img_fishes[6];
    int caughtFishIndex;
    QString fishNames[6];

    // 【新增】全新素材资源
    QPixmap img_combatBg;
    QPixmap img_textbox;
    QPixmap img_exit;
    QPixmap img_boxClosed, img_boxOpen[5];
    QPixmap img_bird[4], img_junimo[4];
    QPixmap img_emoAnim[4], img_emos[15];

    QRect exitBtnRect; // 退出按钮区域

    QTimer *timer;
    QTimer *fastTimer;
};

#endif // GAMEWINDOW_H

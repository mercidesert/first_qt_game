#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>
#include <QTimer>
#include <QKeyEvent>
#include <QPixmap>

enum Season { Winter, Spring, Summer, Fall, NextWinter };
enum CropType { None, Strawberry, Sunflower };
enum CropStage { Seed, Seedling, Grown, Fruiting };
enum Direction { Up, Down, Left, Right };

struct Tile {
    bool isPlowed = false;
    CropType crop = None;
    CropStage stage = Seed;
    int growthTimer = 0;
};

struct Inventory {
    int strawberrySeeds = 0;
    int sunflowerSeeds = 0;
    int strawberryHarvested = 0;
    int sunflowerHarvested = 0;
};

struct Player {
    int x = 5;
    int y = 5;
    Direction dir = Down;
    CropType selectedSeed = Strawberry;
};

class GameWindow : public QWidget {
    Q_OBJECT

public:
    GameWindow(QWidget *parent = nullptr);
    ~GameWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void gameTick();

private:
    void handleAction();
    void updateCrops();
    void changeSeason();
    void drawUI(QPainter &painter);
    void drawSettlement(QPainter &painter);

    static const int TILE_W = 32;
    static const int TILE_H = 64;
    static const int MAP_COLS = 20;
    static const int MAP_ROWS = 10;

    Tile map[MAP_ROWS][MAP_COLS];
    Player player;
    Inventory inventory;

    QTimer *timer;
    Season currentSeason;
    int timeTicks;
    bool isGameOver;

    // --- 新增：收获动画状态变量 ---
    bool isHarvesting;
    int harvestAnimTimer;

    // 图片资源
    QPixmap img_bgWinter, img_bgSpring, img_bgSummer, img_bgFall;
    QPixmap img_soilPlowed;
    QPixmap img_playerUp, img_playerDown, img_playerLeft, img_playerRight;
    QPixmap img_playerHarvest; // 新增：收获动作图片
    QPixmap img_strawSeed, img_strawSeedling, img_strawGrown, img_strawFruiting;
    QPixmap img_sunSeed, img_sunSeedling, img_sunGrown, img_sunFruiting;
};

#endif // GAMEWINDOW_H

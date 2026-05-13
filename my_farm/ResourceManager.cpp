#include "ResourceManager.h"
#include "GlobalConfig.h"

ResourceManager& ResourceManager::instance() { static ResourceManager inst; return inst; }

void ResourceManager::loadAll() {
    auto ls =[](QString path) { return QPixmap(path).scaled(TILE_W, TILE_H, Qt::IgnoreAspectRatio, Qt::FastTransformation); };
    auto lBg =[](QString path) { return QPixmap(path).scaled(640, 640, Qt::IgnoreAspectRatio, Qt::SmoothTransformation); };
    auto lKeep =[](QString path, int w, int h) { return QPixmap(path).scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation); };

    bgWinter = ls(":/images/bg_winter.png"); bgSpring = ls(":/images/bg_spring.png");
    bgSummer = ls(":/images/bg_summer.png"); bgFall = ls(":/images/bg_fall.png");
    soilPlowed = ls(":/images/soil_plowed.png"); playerHarvest = ls(":/images/player_harvest.png");
//农田系统背景
    QString dirNames[4] = {"up", "down", "left", "right"};
    for (int d = 0; d < 4; ++d) {
        for (int f = 0; f < 4; ++f) { playerWalk[d][f] = ls(QString(":/images/player_%1_%2.png").arg(dirNames[d]).arg(f + 1)); }
    }
//人物移动逻辑加载
    strawSeed = ls(":/images/straw_1_seed.png"); strawSeedling = ls(":/images/straw_2_seedling.png");
    strawGrown = ls(":/images/straw_3_grown.png"); strawFruiting = ls(":/images/straw_4_fruiting.png");
    sunSeed = ls(":/images/sun_1_seed.png"); sunSeedling = ls(":/images/sun_2_seedling.png");
    sunGrown = ls(":/images/sun_3_grown.png"); sunFruiting = ls(":/images/sun_4_fruiting.png");
//作物生长状态图
    titleBg = lBg(":/images/title.png"); homeBg = lBg(":/images/home.png");
    feastBg = lBg(":/images/feast.png"); riverBg = lBg(":/images/river_bg.png");
    combatBg = lBg(":/images/combat_bg.png"); textbox = lBg(":/images/textbox.png");
    easterBg = lBg(":/images/eastern.png");
//各系统主页图
    fishIcon = lKeep(":/images/fish_icon.png", 30, 30); envelope = QPixmap(":/images/envelope.png");
    enemy = lKeep(":/images/enemy.png", 128, 128); note = lKeep(":/images/note.png", 32, 32);
    egg = lKeep(":/images/egg.png", 12, 12); sticker = lKeep(":/images/sticker.png", 128, 64);
    exitBtn = lKeep(":/images/exit.png", 64, 54); boxClosed = lKeep(":/images/box_closed.png", 48, 64);
//小型贴图
    for(int i=0; i<5; i++) boxOpen[i] = lKeep(QString(":/images/box_o%1.png").arg(i+1), 48, 64);
    for(int i=0; i<4; i++) bird[i] = lKeep(QString(":/images/bird_%1.png").arg(i+1), 64, 64);
    for(int i=0; i<4; i++) junimo[i] = lKeep(QString(":/images/junimo_%1.png").arg(i+1), 64, 64);
    for(int i=0; i<4; i++) emoAnim[i] = lKeep(QString(":/images/emo_%1.png").arg(i+1), 32, 32);
    for(int i=0; i<15; i++) emos[i] = lKeep(QString(":/images/emo%1.png").arg(i+1), 32, 32);
    for(int i=0; i<6; i++) homeVariants[i] = lBg(QString(":/images/home_%1.png").arg(i+1));
    for(int i=0; i<6; i++) fishes[i] = lKeep(QString(":/images/fish_%1.png").arg(i+1), 96, 96);
//动画加载
    for(int i=0; i<4; i++) {
        catDown[i] = lKeep(QString(":/images/cat_down%1.png").arg(i+1), 48, 48);
        catUp[i]   = lKeep(QString(":/images/cat_up%1.png").arg(i+1), 48, 48);
        catLeft[i] = lKeep(QString(":/images/cat_left%1.png").arg(i+1), 48, 48);
        catRight[i]= lKeep(QString(":/images/cat_right%1.png").arg(i+1), 48, 48);
        catLie[i]  = lKeep(QString(":/images/cat_lie%1.png").arg(i+1), 48, 48);
        catLick[i] = lKeep(QString(":/images/cat_lick%1.png").arg(i+1), 48, 48);
        catSit[i]  = lKeep(QString(":/images/cat_sit%1.png").arg(i+1), 48, 48);
    }
//猫咪多状态加载
    for(int i=0; i<2; i++) {
        catCurl[i] = lKeep(QString(":/images/cat_curl%1.png").arg(i+1), 48, 48);
        catJump[i] = lKeep(QString(":/images/cat_jump%1.png").arg(i+1), 48, 48);
    }
}
//资源加载文件

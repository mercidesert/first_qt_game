#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H
#include <QPixmap>

class ResourceManager {
public:
    static ResourceManager& instance();
    void loadAll();

    QPixmap bgWinter, bgSpring, bgSummer, bgFall, soilPlowed, playerHarvest;
    QPixmap strawSeed, strawSeedling, strawGrown, strawFruiting;
    QPixmap sunSeed, sunSeedling, sunGrown, sunFruiting;
    QPixmap playerWalk[4][4];

    QPixmap titleBg, homeBg, feastBg, riverBg, combatBg, easterBg;
    QPixmap fishIcon, envelope, textbox, exitBtn, enemy, note, egg, sticker;
    QPixmap boxClosed, boxOpen[5], bird[4], junimo[4], emoAnim[4], emos[15];
    QPixmap homeVariants[6], fishes[6];

    QPixmap catDown[4], catUp[4], catLeft[4], catRight[4];
    QPixmap catLie[4], catLick[4], catCurl[2], catSit[4], catJump[2];

private:
    ResourceManager() = default;
};
#endif
//资源管理器单例类，统一加载和管理游戏中所有图片资源，方便各场景通过静态接口获取QPixmap进行绘制

#include "FarmCat.h"
#include "ResourceManager.h"
#include <QRandomGenerator>

FarmCat::FarmCat() : x(300), y(300), state(7), frame(0), animTimer(0), actionTimer(30), emoteState(0), emoteTimer(0), emoteId(0), width(48), height(48) {}
//初始化

void FarmCat::update() {
    auto rng = QRandomGenerator::global();
    actionTimer--;
    if (actionTimer <= 0) {
        state = rng->bounded(9);
        actionTimer = rng->bounded(30, 150);
        frame = 0;
    }
    //更新逻辑，精确到帧

    float speed = 1.5f;
    if (state == 0) y += speed; else if (state == 1) y -= speed;
    else if (state == 2) x -= speed; else if (state == 3) x += speed;

    // 边界检测
    if (x < 0) { x = 0; state = 3; }
    if (x > 600 - width) { x = 600 - width; state = 2; }
    if (y < 0) { y = 0; state = 0; }
    if (y > 600 - height) { y = 600 - height; state = 1; }
    //移动与边界逻辑

    animTimer++;
    if (animTimer > 5) {
        animTimer = 0;
        int maxFrames = (state == 6 || state == 8) ? 2 : 4;
        frame = (frame + 1) % maxFrames;
    }
    //动画帧切换

    if (emoteState >= 1 && emoteState <= 4) {
        emoteTimer++; if (emoteTimer >= 3) { emoteState++; emoteTimer = 0; }
    } else if (emoteState == 5) {
        emoteTimer++; if (emoteTimer >= 45) emoteState = 0;
    }
}
//表情加载更新

void FarmCat::draw(QPainter &painter) {
    QPixmap* arr = ResourceManager::instance().catSit;
    if (state == 0) arr = ResourceManager::instance().catDown; else if (state == 1) arr = ResourceManager::instance().catUp;
    else if (state == 2) arr = ResourceManager::instance().catLeft; else if (state == 3) arr = ResourceManager::instance().catRight;
    else if (state == 4) arr = ResourceManager::instance().catLie; else if (state == 5) arr = ResourceManager::instance().catLick;
    else if (state == 6) arr = ResourceManager::instance().catCurl; else if (state == 8) arr = ResourceManager::instance().catJump;

    //图片加载失效调试
    if (!arr[frame].isNull()) painter.drawPixmap(x, y, width, height, arr[frame]);
    else { painter.setBrush(Qt::magenta); painter.drawRect(x, y, width, height); }
}

void FarmCat::drawEmote(QPainter &painter) {
    // 表情绘制位置适配48x48猫咪
    int drawY = y - height + 16;
    int drawX = x + (width - 32) / 2;

    if (emoteState >= 1 && emoteState <= 4 && !ResourceManager::instance().emoAnim[emoteState-1].isNull()) {
        painter.drawPixmap(drawX, drawY, ResourceManager::instance().emoAnim[emoteState-1]);
    } else if (emoteState == 5 && !ResourceManager::instance().emos[emoteId].isNull()) {
        painter.drawPixmap(drawX, drawY, ResourceManager::instance().emos[emoteId]);
    }
}
//表情绘制

bool FarmCat::checkClick(int mx, int my) {
    if (QRect(x, y, width, height).contains(mx, my)) {
        if (emoteState == 0) { emoteState = 1; emoteTimer = 0; emoteId = QRandomGenerator::global()->bounded(15); }
        return true;
    }
    return false;
}
//鼠标点击检测

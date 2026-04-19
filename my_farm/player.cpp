#include "player.h"
#include <QPixmap>

Player::Player(int size) : playerSize(size) {
    updateAppearance("down");
    setZValue(10); // 确保玩家在土地上方
}

void Player::updateAppearance(QString direction) {
    // 这里暂时只用一张图，你可以根据 direction 加载 player_up.png 等
    QPixmap pix(":/images/player_down.png");
    setPixmap(pix.scaled(playerSize, playerSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

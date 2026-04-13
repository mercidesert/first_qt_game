#include "player.h"

Player::Player(int size) {
    setRect(0, 0, size, size);
    setBrush(QBrush(Qt::blue)); // 玩家是蓝色方块
    setZValue(1); // 确保玩家画在土地的上方
}

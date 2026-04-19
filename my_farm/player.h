#ifndef PLAYER_H
#define PLAYER_H

#include <QGraphicsPixmapItem>

class Player : public QGraphicsPixmapItem {
public:
    Player(int size);
    void updateAppearance(QString direction); // 可以根据方向换图

private:
    int playerSize;
};

#endif

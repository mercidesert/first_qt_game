#ifndef FARMTILE_H
#define FARMTILE_H

#include <QGraphicsPixmapItem>

class FarmTile : public QGraphicsPixmapItem {
public:
    enum TileState { Untilled, Tilled, Planted, Mature };

    FarmTile(int x, int y, int size);
    bool interact(); // 交互逻辑
    void grow();     // 生长逻辑

private:
    TileState currentState;
    int tileSize;
    void updateTexture(); // 根据状态更新图片
};

#endif

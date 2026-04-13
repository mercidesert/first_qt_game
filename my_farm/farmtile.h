#ifndef FARMTILE_H
#define FARMTILE_H

#include <QGraphicsRectItem>
#include <QBrush>

// 定义土地的四种状态
enum TileState {
    Untilled,   // 荒地
    Tilled,     // 已开垦
    Planted,    // 已播种
    Mature      // 已成熟（可收获）
};

class FarmTile : public QGraphicsRectItem {
public:
    FarmTile(int x, int y, int size);

    void interact(); // 玩家按空格时的互动逻辑
    void grow();     // 每天/每隔一段时间的生长逻辑

    TileState currentState;

private:
    void updateColor(); // 根据状态改变颜色（以后这里换成改图片）
};

#endif // FARMTILE_H

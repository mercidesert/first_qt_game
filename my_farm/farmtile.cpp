#include "farmtile.h"

FarmTile::FarmTile(int x, int y, int size) {
    setRect(x, y, size, size); // 设置方块大小和位置
    currentState = Untilled;   // 默认是荒地
    updateColor();
}

void FarmTile::interact() {
    // 核心交互逻辑：状态机转换
    switch (currentState) {
    case Untilled:
        currentState = Tilled; // 锄地
        break;
    case Tilled:
        currentState = Planted; // 播种
        break;
    case Planted:
        // 还没熟，没反应，或者提示“正在生长”
        break;
    case Mature:
        currentState = Untilled; // 收获！变回荒地（你可以加个金币增加的逻辑）
        break;
    }
    updateColor();
}

void FarmTile::grow() {
    // 如果种了种子，就让它成熟
    if (currentState == Planted) {
        currentState = Mature;
        updateColor();
    }
}

void FarmTile::updateColor() {
    switch (currentState) {
    case Untilled: setBrush(QBrush(QColor(100, 200, 100))); break; // 草地绿
    case Tilled:   setBrush(QBrush(QColor(139, 69, 19))); break;   // 泥土褐
    case Planted:  setBrush(QBrush(QColor(173, 255, 47))); break;  // 嫩芽绿
    case Mature:   setBrush(QBrush(QColor(255, 215, 0))); break;   // 丰收黄
    }
}

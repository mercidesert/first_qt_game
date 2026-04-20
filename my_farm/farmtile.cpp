#include "farmtile.h"
#include <QPixmap>

FarmTile::FarmTile(int x, int y, int size) : tileSize(size) {
    setPos(x, y);
    currentState = Untilled;
    updateTexture();
}

bool FarmTile::interact() {
    bool harvested = false; // 默认没有收获

    switch (currentState) {
    case Untilled:
        currentState = Tilled;
        break;
    case Tilled:
        currentState = Planted;
        break;
    case Planted:
        // 正在生长，不能收获
        break;
    case Mature:
        currentState = Untilled; // 收获后变回荒地
        harvested = true;        // 标记收获成功！
        break;
    }
    updateTexture();
    return harvested; // 告诉 Scene 是否加分
}

void FarmTile::grow() {
    if (currentState == Planted) {
        currentState = Mature;
        updateTexture();
    }
}

void FarmTile::updateTexture() {
    QString path;
    switch (currentState) {
    case Untilled: path = ":/images/grass.png"; break;
    case Tilled:   path = ":/images/dirt.png"; break;
    case Planted:  path = ":/images/sprout.png"; break;
    case Mature:   path = ":/images/mature.png"; break;
    }

    QPixmap pix(path);
    // 如果没有图片，Qt会显示空白，这里缩放图片匹配格子大小
    setPixmap(pix.scaled(tileSize, tileSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

#include "farmtile.h"
#include <QPixmap>

FarmTile::FarmTile(int x, int y, int size) : tileSize(size) {
    setPos(x, y);
    currentState = Untilled;
    updateTexture();
}

void FarmTile::interact() {
    switch (currentState) {
    case Untilled: currentState = Tilled; break;
    case Tilled:   currentState = Planted; break;
    case Mature:   currentState = Untilled; break; // 收割后变回荒地
    default: break;
    }
    updateTexture();
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

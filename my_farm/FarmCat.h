#ifndef FARMCAT_H
#define FARMCAT_H

#include <QPixmap>
#include <QPainter>
#include <QRect>

class FarmCat {
public:
    FarmCat();
    void update();
    void draw(QPainter &painter);
    void drawEmote(QPainter &painter);
    bool checkClick(int mx, int my);

    float x, y;
    int state;
    int frame;
    int animTimer, actionTimer;
    int emoteState, emoteTimer, emoteId;
    int width=48;
    int height=48;
};

#endif
//定义农场里的猫类，设定动画与动作、交互控制等

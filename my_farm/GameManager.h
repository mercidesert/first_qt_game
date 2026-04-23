#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QGraphicsScene>
#include <QTimer>
#include <QKeyEvent>
#include <QVector>
#include "farmtile.h"
#include "player.h"

class GameScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit GameScene(QObject *parent = nullptr);

signals:
    void scoreChanged(int newScore); // 当分数改变时通知 MainWindow

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onDayPass();

private:
    const int TILE_SIZE = 48;
    const int GRID_WIDTH = 10;
    const int GRID_HEIGHT = 10;

    Player *player;
    QVector<FarmTile*> farmGrid;
    QTimer *dayTimer;
    int score = 0;
};

#endif

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gamescene.h"
#include <QGraphicsView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 实例化游戏场景
    GameScene *scene = new GameScene(this);

    // 创建视图来显示场景
    QGraphicsView *view = new QGraphicsView(scene, this);

    // 关掉滚动条，固定视角
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 把游戏视图设置为窗口的中心部件
    setCentralWidget(view);

    // 稍微调大一点窗口，刚好装下 10x10 格子 (400x400)
    resize(450, 450);
}

MainWindow::~MainWindow()
{
    delete ui;
}

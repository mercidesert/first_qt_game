#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // 1. 创建场景
    scene = new GameScene(this);

    // 2. 创建视图并设置场景
    view = new QGraphicsView(scene, this);

    // 3. 将视图设置为 MainWindow 的中心组件
    setCentralWidget(view);

    // 设置窗口大小
    setFixedSize(600, 600);
    setWindowTitle("Stardew Valley Qt");
}

#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMediaPlayer>   // 媒体播放器，播放背景音乐
#include <QAudioOutput>   // 音频输出，控制音量
#include <QWidget>        // 窗口基类
#include <QTimer>         // 定时器，驱动游戏循环
#include <QRect>          // 矩形区域，用于按钮点击检测
#include "IScene.h"       // 场景接口类

class GameWindow : public QWidget {
    Q_OBJECT

public:
    GameWindow(QWidget *parent = nullptr);   // 构造函数：初始化窗口、资源、场景、定时器、音乐
    ~GameWindow();                           // 析构函数：清理场景对象

    void changeScene(int stateIndex);        // 切换场景（0主菜单 1家园 2节日 3农场 4钓鱼 5战斗 6彩蛋）

protected:
    // 事件处理（重载自QWidget）
    void paintEvent(QPaintEvent *event) override;        // 绘制事件：绘制场景、HUD、消息框
    void keyPressEvent(QKeyEvent *event) override;       // 键盘按下：ESC返回主菜单，其余转发给场景
    void keyReleaseEvent(QKeyEvent *event) override;     // 键盘释放：过滤长按连发事件
    void mousePressEvent(QMouseEvent *event) override;   // 鼠标按下：检测退出按钮，其余转发给场景

private slots:
    void gameTick();      // 慢速定时器（200ms）：作物生长、季节切换、全局消息
    void fastTick();      // 快速定时器（33ms）：动画更新、物理模拟、触发重绘

private:
    // 辅助绘制函数
    void drawGlobalHUD(QPainter &painter);                // 绘制金币、体力条、受伤状态
    void drawTextbox(QPainter &painter, int x, int y, int w, int h, QString text);  // 绘制对话框

    // 成员变量
    int currentState;           // 当前场景编号
    IScene* scenes[7];          // 7个场景对象指针

    QTimer *timer;              // 慢速定时器（200ms）
    QTimer *fastTimer;          // 快速定时器（33ms）
    QRect exitBtnRect;          // 退出按钮区域

    QMediaPlayer *bgmPlayer;    // 背景音乐播放器
    QAudioOutput *audioOutput;  // 音频输出（控制音量）
};

#endif
//游戏的主窗口类，负责管理场景切换、定时器刷新、用户输入处理和全局界面绘制

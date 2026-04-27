QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StardewFarm
TEMPLATE = app

SOURCES += main.cpp GameWindow.cpp
HEADERS += GameWindow.h
RESOURCES += res.qrc

DISTFILES += \
    images/bg_fall.png \
    images/bg_spring.png \
    images/bg_summer.png \
    images/bg_winter.png \
    images/player_down.png \
    images/player_down_1.png \
    images/player_down_2.png \
    images/player_down_3.png \
    images/player_down_4.png \
    images/player_harvest.png \
    images/player_left.png \
    images/player_left_1.png \
    images/player_left_2.png \
    images/player_left_3.png \
    images/player_left_4.png \
    images/player_right.png \
    images/player_right_1.png \
    images/player_right_2.png \
    images/player_right_3.png \
    images/player_right_4.png \
    images/player_up.png \
    images/player_up_1.png \
    images/player_up_2.png \
    images/player_up_3.png \
    images/player_up_4.png \
    images/soil_plowed.png \
    images/straw_1_seed.png \
    images/straw_2_seedling.png \
    images/straw_3_grown.png \
    images/straw_4_fruiting.png \
    images/sun_1_seed.png \
    images/sun_2_seedling.png \
    images/sun_3_grown.png \
    images/sun_4_fruiting.png

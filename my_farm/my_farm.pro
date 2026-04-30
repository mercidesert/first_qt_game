QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StardewValleyLite
TEMPLATE = app

# 源文件 (.cpp)
SOURCES += \
    FarmCat.cpp \
    GameData.cpp \
    GameWindow.cpp \
    ResourceManager.cpp \
    Scenes.cpp \
    main.cpp

# 头文件 (.h)
HEADERS += \
    FarmCat.h \
    GameData.h \
    GameWindow.h \
    GlobalConfig.h \
    IScene.h \
    ResourceManager.h \
    Scenes.h

# 资源文件 (.qrc)
RESOURCES += \
    res.qrc

# 演示模式
# DEFINES += DEMO_MODE

QT       += core gui \
            network


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat


CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    floatingbutton.cpp \
    machinewnd.cpp \
    main.cpp \
    nightmodechecker.cpp \
    shutdownpara.cpp \
    shutdowntask.cpp \
    shutmgrwnd.cpp \
    logwnd.cpp \
    shutprocess.cpp \
    starterchecker.cpp

HEADERS += \
    darkqss.h \
    floatingbutton.h \
    machinewnd.h \
    nightmodechecker.h \
    shutdownpara.h \
    shutdowntask.h \
    shutmgrwnd.h \
    logwnd.h \
    shutprocess.h \
    starterchecker.h

FORMS += \
    floatingbutton.ui \
    machinewnd.ui \
    shutmgrwnd.ui \
    logwnd.ui

# TRANSLATIONS += \
    # ShutMgr_R_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

# 如果你有WDK，就不用这一句
# LIBS += \
    # ???\Qt\Tools\mingw1120_64\x86_64-w64-mingw32\lib\libpowrprof.a

QMAKE_CXXFLAGS += -Zm512


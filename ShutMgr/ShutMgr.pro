QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    chosemachine.cpp \
    logwindow.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    MyFuncs.h \
    chosemachine.h \
    logwindow.h \
    mainwindow.h \
    uac.h

FORMS += \
    chosemachine.ui \
    logwindow.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

DISTFILES +=

#RC_FILE = version.rc

#OTHER_FILES += version.rc

VERSION = 1.14.514.1919

RC_ICONS = Programico.ico

QMAKE_TARGET_COMPANY =""

QMAKE_TARGET_DESCRIPTION = "关机管理器"

QMAKE_TARGET_COPYRIGHT = "版权所有(C)2022 Command Prompt"

QMAKE_TARGET_PRODUCT = "关机管理器"

RC_LANG = 0x0800

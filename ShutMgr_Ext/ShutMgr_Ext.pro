QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ShutMgr_Ext.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ShutMgr_Ext.h

VERSION = 1.14.514.1919

#RC_ICONS = app.ico

QMAKE_TARGET_COMPANY =""

QMAKE_TARGET_DESCRIPTION = "�ػ�������_��չ"

QMAKE_TARGET_COPYRIGHT = "��Ȩ����(C)2022 Command Prompt"

QMAKE_TARGET_PRODUCT = "�ػ�������"

RC_LANG = 0x0800


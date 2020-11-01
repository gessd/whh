#-------------------------------------------------
#
# Project created by QtCreator 2020-07-10T20:30:28
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

win32:DEFINES += _USER_DRV_API_

TARGET = SysSafe
TEMPLATE = app

#程序单例运行使用插件
include(./qtsingleapplication/src/qtsingleapplication.pri)

#CONFIG(debug, debug|release){
    #显示调试信息
    #CONFIG +=console
#}

TRANSLATIONS = ./translator/language.ts

SOURCES += main.cpp\
        mainwindow.cpp \
    createuserdialog.cpp \
    usermanage.cpp \
    contorllthread.cpp \
    xmessagebox.cpp \
    setconfig.cpp \
    dfcx/dfcx_drv.cpp \
    dfcx/userservice.cpp

HEADERS  += mainwindow.h \
    createuserdialog.h \
    usermanage.h \
    styledefine.h \
    contorllthread.h \
    xmessagebox.h \
    setconfig.h \
    dfcx/dfcx_common.h \
    dfcx/dfcx_drv.h \
    dfcx/userservice.h

FORMS    += mainwindow.ui \
    createuserdialog.ui \
    xmessagebox.ui

RESOURCES +=
    syssafe.qrc

INCLUDEPATH += $$PWD/dfcx

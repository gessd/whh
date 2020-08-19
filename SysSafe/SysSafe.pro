#-------------------------------------------------
#
# Project created by QtCreator 2020-07-10T20:30:28
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += _USER_DRV_API_

TARGET = SysSafe
TEMPLATE = app

#程序单例运行使用插件
include(./qtsingleapplication/src/qtsingleapplication.pri)

#显示调试信息
#CONFIG +=console

TRANSLATIONS = language.ts

SOURCES += main.cpp\
        mainwindow.cpp \
    createuserdialog.cpp \
    usermanage.cpp \
    contorllthread.cpp \
    xmessagebox.cpp \
    setconfig.cpp

HEADERS  += mainwindow.h \
    createuserdialog.h \
    usermanage.h \
    styledefine.h \
    contorllthread.h \
    xmessagebox.h \
    setconfig.h

FORMS    += mainwindow.ui \
    createuserdialog.ui \
    xmessagebox.ui

RESOURCES += \
    syssafe.qrc

LIBS += -L$$PWD/drv/lib/

contains(DEFINES,_USER_DRV_API_){
LIBS += -lsqlite \
        -lXGComApi \
        -ldfcx_drv
}

INCLUDEPATH += $$PWD/drv/include
DEPENDPATH += $$PWD/drv/lib


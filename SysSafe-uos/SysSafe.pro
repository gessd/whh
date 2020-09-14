#-------------------------------------------------
#
# Project created by QtCreator 2020-07-10T20:30:28
#
#-------------------------------------------------

QT       += core gui dbus network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += APP_API_MAJOR=0  \
            APP_API_MINOR=11    \
            APP_API_FUNC=0

TARGET = SysSafe
TEMPLATE = app

#程序单例运行使用插件
include(./qtsingleapplication/src/qtsingleapplication.pri)


TRANSLATIONS = language.ts

SOURCES += main.cpp\
        mainwindow.cpp \
    xmessagebox.cpp \
    setconfig.cpp \
    customtype.cpp

HEADERS  += mainwindow.h \
    styledefine.h \
    xmessagebox.h \
    setconfig.h \
    customtype.h

FORMS    += mainwindow.ui \
    xmessagebox.ui

#LIBS += -ldl

RESOURCES += \
    syssafe.qrc

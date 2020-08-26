#-------------------------------------------------
#
# Project created by QtCreator 2020-08-26T15:00:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DeviceStatus
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#程序单例运行使用插件
include(../qtsingleapplication/src/qtsingleapplication.pri)


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    ../setconfig.cpp

HEADERS += \
        mainwindow.h \
    ../setconfig.h

FORMS += \
        mainwindow.ui
RESOURCES += ../syssafe.qrc

LIBS += -L$$PWD/../drv/lib/

LIBS += -lsqlite \
        -lXGComApi \
        -ldfcx_drv

INCLUDEPATH += $$PWD/../drv/include
DEPENDPATH += $$PWD/../drv/lib

QT -= gui
QT += dbus

TARGET = df100-service

CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#如果有依赖库,可以在工程目录下新建lib目录，然后用-lxxxxxxx(替换成对应的库名）
LIBS += -L./lib -ldl -lsqlite3

CONFIG += c++11

SOURCES += \
        main.cpp \
        df100dbusservice.cpp \
	userservice.cpp \
	VerifyThread.cpp 

HEADERS += \
        df100dbusservice.h \
	userservice.h \
	VerifyThread.h

target.path = /usr/bin/

install_fingerconf.files = ./com.dfcx.df100.VFingerprint.service
install_fingerconf.path = /usr/share/dbus-1/system-services/

install_dbusconf.files = ./com.dfcx.df100.VFingerprint.conf
install_dbusconf.path = /usr/share/dbus-1/system.d/

install_adaptconf.files = ./vfAdaptation.conf
install_adaptconf.path = /usr/share/deepin-authentication/interfaces/

#用于安装依赖的库
install_lib.file = ./lib/*
install_lib.path = /usr/lib/

INSTALLS += target install_fingerconf install_dbusconf install_adaptconf install_lib

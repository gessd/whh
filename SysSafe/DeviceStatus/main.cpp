#include "mainwindow.h"
#include <QApplication>
#include "QtSingleApplication"

#define _SingleAppId_ "DeviceProgram"

int main(int argc, char *argv[])
{
    QtSingleApplication a(_SingleAppId_, argc, argv);
    if (a.isRunning()){
        a.sendMessage("raise_window_noop");
        return EXIT_SUCCESS;
    }
    MainWindow w;
    if(false == w.startCheckDevice()){
        return -1;
    }
    return a.exec();
}

#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtCore/QProcess>
#include "xmessagebox.h"
#include "QtSingleApplication"
#include <QFile>

//只能启动单实例程序
#define _SingleProgram_
#define _SingleAppId_ "FingerProgram"

int main(int argc, char *argv[])
{
#ifdef _SingleProgram_
    QtSingleApplication app(_SingleAppId_, argc, argv);
    if (app.isRunning()){
        app.sendMessage("raise_window_noop");
        return EXIT_SUCCESS;
    }
    MainWindow w;
    app.setActivationWindow(&w);
#else
    QApplication app(argc, argv);
    MainWindow w;
#endif
    w.show();
    w.sysInit();
    //if(0 != w.sysInit()){
    //    XMessageBox::warning(&w, QObject::tr("警告"), QObject::tr("系统初始化失败，无法使用！"));
    //    return 0;
    //}
    w.showFingerInfo();
    int nExec = app.exec();
    w.sysUnInit();
    if (nExec == RETCODE_RESTART){
        QProcess::startDetached(QApplication::applicationFilePath(), QStringList());
        return 0;
    }
    return nExec;

}

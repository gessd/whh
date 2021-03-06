#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtCore/QProcess>
#include <QtCore/QTranslator>
#include "xmessagebox.h"
#include "QtSingleApplication"
#include "styledefine.h"
#include "setconfig.h"
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

    QString qstrTranlatorFile = QString(":/translator/%1");
    int nLanguage = SetConfig::getSetValue(_LanguageSet, 0).toInt();
    switch (nLanguage) {
    case 0: break;
    case 1: qstrTranlatorFile.arg("zh_tw.qm"); break;
    case 2: qstrTranlatorFile.arg("eng.qm"); break;
    case 3: qstrTranlatorFile.arg("mw.qm"); break;
    default: break;
    }
    //加载翻译文件
    QTranslator translator;
    if(translator.load(qstrTranlatorFile)){
        QApplication::installTranslator(&translator);
    }

    MainWindow w;
    app.setActivationWindow(&w);
#else
    QApplication app(argc, argv);
    MainWindow w;
#endif
    w.show();
    if(0 != w.sysInit()){
        XMessageBox::warning(&w, QObject::tr("警告"), QString("<font color=%1>"+QObject::tr("系统初始化失败，无法使用！")+"</font>").arg(SetConfig::getSetValue(_MessageErrorColor, "#FF0000")));
        return 0;
    }
    w.showUserName();

    int nExec = app.exec();
    w.sysUnInit();
    if (nExec == RETCODE_RESTART){
        QProcess::startDetached(QApplication::applicationFilePath(), QStringList());
        return 0;
    }
    return nExec;
}

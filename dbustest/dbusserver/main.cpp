#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include "dbusdefine.h"

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    //建立到session bus的连接
    QDBusConnection connection = QDBusConnection::sessionBus();
    //在session bus上注册的服务
    if(!connection.registerService(_DBUS_Service_)){
        qDebug() << "error:" << connection.lastError().message();
        return -1;
    }
    //注册对象，把类Object所有槽函数和信号导出为object的method
    if (!connection.registerObject(_DBUS_Paht_, &w, QDBusConnection::ExportAllSlots|QDBusConnection::ExportAllSignals)){
        qDebug() << "error:" << connection.lastError().message();
        return -1;
    }

    return a.exec();
}

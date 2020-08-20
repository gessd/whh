#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDBusArgument>
#include <unistd.h>
#include <pwd.h>

#include "customtype.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /* 连接 DBus Daemon */
    serviceInterface = new QDBusInterface(DBUS_SERVICE, DBUS_PATH,
                                          DBUS_INTERFACE,
                                          QDBusConnection::systemBus());
    serviceInterface->setTimeout(2147483647); /* 微秒 */

    connect(serviceInterface, SIGNAL(USBDeviceHotPlug(int, int, int)),
            this, SLOT(onUSBDeviceHotPlug(int,int,int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    struct passwd *pwd;
    pwd = getpwuid(getuid());
    QString username = QString(pwd->pw_name);
    qDebug()<<"----username"<<username;
}

/**
 * @brief 获取设备列表并存储起来备用
 */
void MainWindow::getDeviceInfo()
{
    QVariant variant;
    QDBusArgument argument;
    QList<QDBusVariant> qlist;
    QDBusVariant item;
    DeviceInfo *deviceInfo;

    /* 返回值为 i -- int 和 av -- array of variant */
    QDBusPendingReply<int, QList<QDBusVariant> > reply = serviceInterface->call("GetDrvList");
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << "GUI:" << reply.error();
        deviceCount = 0;
        return;
    }

    /* 解析 DBus 返回值，reply 有两个返回值，都是 QVariant 类型 */
    variant = reply.argumentAt(0); /* 得到第一个返回值 */
    deviceCount = variant.value<int>(); /* 解封装得到设备个数 */
    variant = reply.argumentAt(1); /* 得到第二个返回值 */
    argument = variant.value<QDBusArgument>(); /* 解封装，获取QDBusArgument对象 */
    argument >> qlist; /* 使用运算符重载提取 argument 对象里面存储的列表对象 */

    for(int i = 0; i < __MAX_NR_BIOTYPES; i++)
        deviceInfosMap[i].clear();;

    for (int i = 0; i < deviceCount; i++) {
        item = qlist[i]; /* 取出一个元素 */
        variant = item.variant(); /* 转为普通QVariant对象 */
        /* 解封装得到 QDBusArgument 对象 */
        argument = variant.value<QDBusArgument>();
        deviceInfo = new DeviceInfo();
        argument >> *deviceInfo; /* 提取最终的 DeviceInfo 结构体 */
        deviceInfosMap[bioTypeToIndex(deviceInfo->biotype)].append(deviceInfo);
    }
}

/**
 * @brief 设备类型到索引的映射
 */
int MainWindow::bioTypeToIndex(int type)
{
    switch(type) {
    case BIOTYPE_FINGERPRINT:
        return 0;
    case BIOTYPE_FINGERVEIN:
        return 1;
    case BIOTYPE_IRIS:
        return 2;
    case BIOTYPE_VOICEPRINT:
        return 3;
    }
    return -1;
}

void MainWindow::on_pushButton_2_clicked()
{
    getDeviceInfo();
}

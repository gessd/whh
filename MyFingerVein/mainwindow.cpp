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

    m_pFingerVeinDeviceINfo = NULL;

    /* 连接 DBus Daemon */
    serviceInterface = new QDBusInterface(DBUS_SERVICE, DBUS_PATH,
                                          DBUS_INTERFACE,
                                          QDBusConnection::systemBus());
    serviceInterface->setTimeout(2147483647); /* 微秒 */

    connect(serviceInterface, SIGNAL(USBDeviceHotPlug(int, int, int)),
            this, SLOT(onUSBDeviceHotPlug(int,int,int)));

    //注册自定义数据类型
    registerCustomTypes();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addDebugText(QString qstrText)
{
    ui->textBrowser->append(qstrText);
}

void MainWindow::on_pushButton_clicked()
{
    struct passwd *pwd;
    pwd = getpwuid(getuid());
    QString username = QString(pwd->pw_name);
    qDebug()<<"----username"<<username;
    addDebugText(QString("--UserName:%1").arg(username));
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

    addDebugText(QString("--device count:%1").arg(deviceCount));
    for (int i = 0; i < deviceCount; i++) {
        item = qlist[i]; /* 取出一个元素 */
        variant = item.variant(); /* 转为普通QVariant对象 */
        /* 解封装得到 QDBusArgument 对象 */
        argument = variant.value<QDBusArgument>();
        deviceInfo = new DeviceInfo();
        argument >> *deviceInfo; /* 提取最终的 DeviceInfo 结构体 */
        deviceInfosMap[bioTypeToIndex(deviceInfo->biotype)].append(deviceInfo);

        if(BIOTYPE_FINGERVEIN == deviceInfo->biotype){
            addDebugText(QString("--device info\n id:%1 全名:%3 缩略名:%2 设备类型:%4")
                         .arg(deviceInfo->device_id).arg(deviceInfo->device_shortname)
                         .arg(deviceInfo->device_fullname).arg(deviceInfo->biotype));
            qDebug()<<"---device "<<i<<deviceInfo<<deviceInfo->device_id<<
                      deviceInfo->device_shortname<<deviceInfo->device_fullname<<deviceInfo->biotype;

            m_pFingerVeinDeviceINfo = deviceInfo;
        }
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

//录入
void MainWindow::on_pushButton_3_clicked()
{
    int uid = getuid();
    QList<QVariant> args;
    //args << drvId << uid << idx << idxName;
    args<<m_pFingerVeinDeviceINfo->device_id<<uid<<1<<"whhhh";
    serviceInterface->callWithCallback("Enroll", args, this,
                        SLOT(enrollCallBack(const QDBusMessage &)),
                        SLOT(errorCallBack(const QDBusError &)));
}

void MainWindow::onUSBDeviceHotPlug(int, int, int)
{
    addDebugText("****function onUSBDeviceHotPlug");
}

void MainWindow::enrollCallBack(const QDBusMessage &reply)
{
    addDebugText("****function enrollCallBack");
    int result;
    result = reply.arguments()[0].value<int>();
    qDebug() << "Enroll result: " << result;
    switch(result) {
    case DBUS_RESULT_SUCCESS: { /* 录入成功 */
        addDebugText("---录入成功");
        break;
    }
    default:
        addDebugText("---录入失败");
        addDebugText(handleErrorResult(result));
        break;
    }
}

void MainWindow::verifyCallBack(const QDBusMessage &reply)
{
    addDebugText("****function verifyCallBack");
}

void MainWindow::searchCallBack(const QDBusMessage &reply)
{
    addDebugText("****function searchCallBack");
}

void MainWindow::StopOpsCallBack(const QDBusMessage &reply)
{
    addDebugText("****function StopOpsCallBack");
}

void MainWindow::errorCallBack(const QDBusError &error)
{
    addDebugText("****function errorCallBack");
    qDebug() << "DBus Error: " << error.message();
    addDebugText(error.message());
}

QString MainWindow::handleErrorResult(int error)
{
    int deviceId = m_pFingerVeinDeviceINfo->device_id;
    switch(error) {
    case DBUS_RESULT_ERROR: {
        //操作失败，需要进一步获取失败原因
        QDBusMessage msg = serviceInterface->call("GetOpsMesg", deviceId);
        if(msg.type() == QDBusMessage::ErrorMessage)
        {
            qDebug() << "UpdateStatus error: " << msg.errorMessage();
            return "接口错误";
        }
        qDebug() << "GetOpsMesg: deviceId--" << deviceId;
        break;
    }
    case DBUS_RESULT_DEVICEBUSY:
        return "设备忙";
        break;
    case DBUS_RESULT_NOSUCHDEVICE:
        return "设备不存在";
        break;
    case DBUS_RESULT_PERMISSIONDENIED:
        return "没有权限";
        break;
    }
    return "未知错误";
}

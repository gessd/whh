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
    qDebug()<<"===="<<qstrText;
}

void MainWindow::on_pushButton_clicked()
{
    struct passwd *pwd;
    pwd = getpwuid(getuid());
    QString username = QString(pwd->pw_name);
    qDebug()<<"----username"<<username;
    addDebugText(QString("--UserName:%1").arg(username));
    ui->lineEditUser->setText(username);
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
    ui->lineEditDevice->setText(QString::number(m_pFingerVeinDeviceINfo->device_id));
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
    int drvId = m_pFingerVeinDeviceINfo->device_id;
    int uid = getuid();
    int idx = ui->lineEditid->text().trimmed().toInt();
    QString idxName = ui->lineEditname->text().trimmed();
    QList<QVariant> args;
    args << drvId << uid << idx << idxName;
    //args<<m_pFingerVeinDeviceINfo->device_id<<uid<<1<<"whhhh";
    serviceInterface->callWithCallback("Enroll", args, this,
                        SLOT(enrollCallBack(const QDBusMessage &)),
                        SLOT(errorCallBack(const QDBusError &)));
}

void MainWindow::onUSBDeviceHotPlug(int int1, int int2, int int3)
{
    addDebugText(QString("****function onUSBDeviceHotPlug %1 %2 %3").arg(int1).arg(int2).arg(int3));
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
    int result;
    result = reply.arguments()[0].value<int>();
    qDebug() << "Verify result: " << result;

    if(result >= 0) {
        addDebugText("Verify successfully");
    } else if(result == DBUS_RESULT_NOTMATCH) {
        addDebugText("Not Match");
    } else {
        addDebugText(handleErrorResult(result));
    }
}

void MainWindow::searchCallBack(const QDBusMessage &reply)
{
    addDebugText("****function searchCallBack");

    int result;
    result = reply.arguments()[0].value<int>();
    qDebug() << "Verify result: " << result;

    if(result > 0) {
        addDebugText("Search Result");
        int count  = result;
        QDBusArgument argument = reply.arguments().at(1).value<QDBusArgument>();
        QList<QVariant> variantList;
        argument >> variantList;
        QList<SearchResult> results;
        for(int i = 0; i < count; i++) {
            QDBusArgument arg =variantList.at(i).value<QDBusArgument>();
            SearchResult ret;
            arg >> ret;
            results.append(ret);
            addDebugText(QString("-- searh info: uid:%1 index:%2 name:%3").arg(ret.uid).arg(ret.index).arg(ret.indexName));
        }
    }
    else if(result >= DBUS_RESULT_NOTMATCH)
        addDebugText("No matching features Found");
    else
        addDebugText(handleErrorResult(result));
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

void MainWindow::errorCallback(QDBusError error)
{
    addDebugText(error.message());
}

void MainWindow::showFeaturesCallback(QDBusMessage callbackReply)
{
    QList<QDBusVariant> qlist;
    FeatureInfo *featureInfo;
    int listsize;

    QList<QVariant> variantList = callbackReply.arguments();
    listsize = variantList[0].value<int>();
    variantList[1].value<QDBusArgument>() >> qlist;
    for (int i = 0; i < listsize; i++) {
        featureInfo = new FeatureInfo;
        qlist[i].variant().value<QDBusArgument>() >> *featureInfo;
        //dataModel->appendData(featureInfo);
        QString qstrInfo = QString("---- uid:%1 biotype:%2 device_shortname:%3 index:%4 index_name:%5")
                .arg(featureInfo->uid).arg(featureInfo->biotype).arg(featureInfo->device_shortname).arg(featureInfo->index).arg(featureInfo->index_name);
        addDebugText(qstrInfo);
    }
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

//验证
void MainWindow::on_pushButton_4_clicked()
{
    int drvId = m_pFingerVeinDeviceINfo->device_id;
    int uid = getuid();
    int idx = ui->lineEditid->text().trimmed().toInt();
    QList<QVariant> args;
    args << drvId << uid << idx;

    serviceInterface->callWithCallback("Verify", args, this,
                        SLOT(verifyCallBack(const QDBusMessage &)),
                        SLOT(errorCallBack(const QDBusError &)));
}

//删除
void MainWindow::on_pushButton_5_clicked()
{
    int drvId = m_pFingerVeinDeviceINfo->device_id;
    int uid = getuid();
    int idxStart = ui->lineEditid->text().trimmed().toInt();
    int idxEnd = -1;
    QDBusPendingReply<int> reply = serviceInterface->call("Clean", drvId, uid, idxStart, idxEnd);
    reply.waitForFinished();
    if (reply.isError()) {
        qWarning() << "DBUS:" << reply.error();
        return;
    }
    addDebugText("----删除完成");
}

//查找
void MainWindow::on_pushButton_6_clicked()
{
    int drvId = m_pFingerVeinDeviceINfo->device_id;
    int uid = getuid();
    int idxStart = 0;
    int idxEnd = -1;
    QList<QVariant> args;
    args << drvId << uid << idxStart << idxEnd;

    serviceInterface->callWithCallback("Search", args, this,
                        SLOT(searchCallBack(const QDBusMessage &)),
                        SLOT(errorCallBack(const QDBusError &)));
}

void MainWindow::on_pushButton_7_clicked()
{
    int drvId = m_pFingerVeinDeviceINfo->device_id;
    int uid = getuid();
    int idxStart = 0;
    int idxEnd = -1;
    QList<QVariant> args;
    args << drvId<< uid << idxStart << idxEnd;

    serviceInterface->callWithCallback("GetFeatureList", args, this,
                        SLOT(showFeaturesCallback(QDBusMessage)),
                        SLOT(errorCallback(QDBusError)));
}

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("client");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addDebugText(QString qstrText)
{
    ui->textBrowser->append(qstrText);
}

void MainWindow::dbusfunc1()
{
    //构造一个method_call消息，服务名称为：com.scorpio.test，对象路径为：/test/objects
    //接口名称为com.scorpio.test.value，method名称为value
    QDBusMessage message = QDBusMessage::createMethodCall(_DBUS_Service_, _DBUS_Paht_, _DBUS_Interface_, "setName");

    // 传递参数
    message << QString("wangjian");

    //发送消息
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    //判断method是否被正确返回
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        //从返回参数获取返回值
        QString value = response.arguments().takeFirst().toString();
        qDebug() << QString("dbus value =  %1").arg(value);
        addDebugText(QString("dbus value =  %1").arg(value));
    }
    else
    {
        qDebug() << "value method called failed!";
        addDebugText("value method called failed!");
    }
}

void MainWindow::dbusfunc2()
{
    /*
    QDBusInterface interface("com.scorpio.test", "/home/lyb/objects",
                             "com.scorpio.test.value",
                             QDBusConnection::sessionBus());
    if (!interface.isValid())
    {
        qDebug() << qPrintable(QDBusConnection::sessionBus().lastError().message());
        exit(1);
    }
    */
    //{
        // 方法一：接收服务端的信号，连接槽函数。服务器对象必须注册QDBusConnection::ExportAllSignals
        //   if (!QDBusConnection::sessionBus().connect("com.scorpio.test", "/home/lyb/objects",
        //                                             "com.scorpio.test.value",
        //                                             "signals_process_value", this,
        //                                             SLOT(slots_process_value(int))))

        //方法二： 接收服务端的信号，连接槽函数。服务器对象必须注册QDBusConnection::ExportAllSignals
        QDBusInterface *pinterface = new QDBusInterface(_DBUS_Service_, _DBUS_Paht_, _DBUS_Interface_,  QDBusConnection::sessionBus());
        QObject::connect(pinterface, SIGNAL(signals_process_value(int,QString,QString)), this, SLOT(onProcessValue(int, QString, QString)));
    //}
        //QDBusReply<QString> reply = pinterface->call("setName", "testname"); //阻塞，直到远程方法调用完成。

    // 这里不阻塞，异步调用。
    QDBusPendingCall async = pinterface->asyncCall("setName", "Brion");
    // 等待结束，async.waitForFinished ()
    //async.waitForFinished();
    //QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(async, this);

    //QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),this, SLOT(callFinishedSlot(QDBusPendingCallWatcher*)));
}

void MainWindow::onProcessValue(int index, QString name, QString qstrTime)
{
    addDebugText(QString("---- function slot onProcessValue %1 %2 %3").arg(index).arg(name).arg(qstrTime));
}

void MainWindow::on_btnConnect_clicked()
{
    dbusfunc2();
}

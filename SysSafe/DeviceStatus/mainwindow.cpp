#include <QDateTime>
#include <QDebug>
#include <QProcess>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../styledefine.h"
#include "../setconfig.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_bInit = false;
    m_pTimerDevice = new QTimer(this);
    connect(m_pTimerDevice, SIGNAL(timeout()), this, SLOT(onTimeOut()));

    //创建菜单
    m_pQuitAction = new QAction(tr("退出"), this);
    m_pShowAction = new QAction(tr("显示"), this);
    //创建状态栏
    m_pTrayIconMenu = new QMenu(this);
    m_pTrayIconMenu->addAction(m_pShowAction);
    m_pTrayIconMenu->addSeparator();
    m_pTrayIconMenu->addAction(m_pQuitAction);

    m_pTrayIcon = new QSystemTrayIcon(this);
    m_pTrayIcon->setContextMenu(m_pTrayIconMenu);
    QIcon icon(QIcon(":/images/heart.png"));
    m_pTrayIcon->setIcon(icon);
    setWindowIcon(icon);
    m_pTrayIcon->show();

    connect(m_pQuitAction, SIGNAL(triggered()), this, SLOT(onCloseWindow()));
    connect(m_pShowAction, SIGNAL(triggered()), this, SLOT(onShowWindow()));

    m_pTcpClient = NULL;
    m_pTcpServer = new QTcpServer(this);
}

MainWindow::~MainWindow()
{
    unInit();
    delete ui;
}

bool MainWindow::startCheckDevice()
{
    m_pTimerDevice->start(10*1000);

    //启动服务监控端口
    if(false == m_pTcpServer->listen(QHostAddress::Any, _TcpPort_)){
        return false;
    }

    int nInit = init();
    if(0 != nInit){
        m_bInit = false;
        return false;
    }

    connect (m_pTcpServer, SIGNAL(newConnection()),this, SLOT(updateStatus()));//有新客户连接
    //m_pTimerDevice->start(10*1000);
    m_bInit = true;
    return true;
}

void MainWindow::onTimeOut()
{
    static unsigned int nTimeLast = 0;
    //if(false == m_bInit) return;
    int nConDev = connectDev();
    if(0 == nConDev) {
        nTimeLast = 0;
        disconnectDev();
        return;
    }
    if(0 == nTimeLast){
        nTimeLast = QDateTime::currentDateTime().toTime_t();
    }
    unsigned int nCurrentTime = QDateTime::currentDateTime().toTime_t();
    unsigned int nMaxTime = SetConfig::getSetValue(_MaxTimeOutDeviceOffline, 60).toUInt();
    unsigned int nGapTime = nCurrentTime-nTimeLast;
    if(nGapTime>nMaxTime){
        qDebug()<<"设备离线"<<nMaxTime<<nGapTime;

        QString qstrTitle = tr("安全登录");
        QString qstrMessage = tr("设备不在线，将执行安全策略");
        QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Warning);
        QIcon ion = windowIcon();
        m_pTrayIcon->showMessage(qstrTitle, qstrMessage, icon);

    }
}

void MainWindow::onShowWindow()
{
    if(m_pTcpClient){
        m_pTcpClient->write("close");
    } else {
        //未连接，需要先启动主程序
        QProcess pro;
        pro.startDetached("SysSafe.exe");
    }
}

void MainWindow::onCloseWindow()
{
    if(m_pTcpClient) m_pTcpClient->write("close");
    close();
}

void MainWindow::onNewConnection()
{
    if(m_pTcpClient) return;
    m_pTcpClient = m_pTcpServer->nextPendingConnection();
    connect(m_pTcpClient, SIGNAL(disconnected()), this, SLOT(onClientUpdateStatus()));
    connect(m_pTcpClient, SIGNAL(readyRead()), this, SLOT(onClientReadMessage()));
}

void MainWindow::onClientUpdateStatus()
{
    disconnect(m_pTcpClient, SIGNAL(disconnected()), this, SLOT(onClientUpdateStatus()));
    disconnect(m_pTcpClient, SIGNAL(readyRead()), this, SLOT(onClientReadMessage()));
    m_pTcpClient = NULL;
}

void MainWindow::onClientReadMessage()
{
    if(NULL == m_pTcpClient) return;
    QByteArray data = m_pTcpClient->readAll();
}

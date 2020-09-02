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
    m_pQuitAction = new QAction(QIcon(":/images/title_close.png"), tr("退出"), this);
    m_pShowAction = new QAction(QIcon(":/images/win.png"), tr("显示"), this);
    //创建状态栏
    m_pTrayIconMenu = new QMenu(this);
    m_pTrayIconMenu->addAction(m_pShowAction);
    m_pTrayIconMenu->addSeparator();
    m_pTrayIconMenu->addAction(m_pQuitAction);

    m_pTrayIcon = new QSystemTrayIcon(this);
    m_pTrayIcon->setContextMenu(m_pTrayIconMenu);
    QIcon icon(QIcon(":/images/win.png"));
    m_pTrayIcon->setIcon(icon);
    setWindowIcon(icon);
    m_pTrayIcon->show();

    connect(m_pQuitAction, SIGNAL(triggered()), this, SLOT(onCloseWindow()));
    connect(m_pShowAction, SIGNAL(triggered()), this, SLOT(onShowWindow()));

    m_pLocalClient = NULL;
    m_pLocalServer = new QLocalServer(this);
}

MainWindow::~MainWindow()
{
    unInit();
    delete ui;
}

bool MainWindow::startCheckDevice()
{
    int nInit = init();
    if(0 != nInit){
        m_bInit = false;
        return false;
    }

    //启动服务监控端口
    if(false == m_pLocalServer->listen(_LocasServerName_)){
        return false;
    }
    qDebug()<<"---- server listen"<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    connect (m_pLocalServer, SIGNAL(newConnection()),this, SLOT(onNewConnection()));//有新客户连接
    m_pTimerDevice->start(3*1000);
    m_bInit = true;
    return true;
}

void MainWindow::onTimeOut()
{
    qDebug()<<"开始检测设备是否在线"<<QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    static unsigned int nTimeLast = 0;
    //if(false == m_bInit) return;
    int nCheck = checkOnline();
    if(nCheck > 0) {
        qDebug()<<"设备在线";
        nTimeLast = 0;
        return;
    }
    if(0 == nTimeLast){
        nTimeLast = QDateTime::currentDateTime().toTime_t();
    }
    unsigned int nCurrentTime = QDateTime::currentDateTime().toTime_t();
    unsigned int nMaxTime = SetConfig::getSetValue(_MaxTimeOutDeviceOffline, 10).toUInt()*60;
    unsigned int nGapTime = nCurrentTime-nTimeLast;
    qDebug()<<"设备离线是否超时"<<nGapTime<<nMaxTime;
    if(nGapTime>nMaxTime){
        qDebug()<<"设备长时间离线"<<nMaxTime<<nGapTime;
        nTimeLast = 0;
        QString qstrTitle = tr("安全登录");
        QString qstrMessage = tr("设备不在线，将执行安全策略");
        QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Warning);
        QIcon ion = windowIcon();
        m_pTrayIcon->showMessage(qstrTitle, qstrMessage, icon);

        int nActionSet = SetConfig::getSetValue(_ActionSet, ActionLockScreen).toInt();
        qDebug()<<"准备执行安全策略"<<nActionSet;
        int nRes = -1;
        switch (nActionSet) {
        case ActionNone:
            break;
        case ActionLockScreen: nRes = lockScreen();
            break;
        case ActionLogout: nRes = logout();
            break;
        case ActionReboot: nRes = reboot();
            break;
        case ActionHalt: nRes = halt();
            break;
        default:
            break;
        }
    }
}

void MainWindow::onShowWindow()
{
    if(m_pLocalClient){
        m_pLocalClient->write(_ShowWindow_);
    } else {
        //未连接，需要先启动主程序
        QProcess pro;
        pro.startDetached(_MainProgramName_);
    }
}

void MainWindow::onCloseWindow()
{
    if(m_pLocalClient) m_pLocalClient->write(_CloseWindow_);
    close();
}

void MainWindow::onNewConnection()
{
    //if(m_pLocalClient) return;
    qDebug()<<"----client connect"<<m_pLocalClient;
    m_pLocalClient = m_pLocalServer->nextPendingConnection();
    connect(m_pLocalClient, SIGNAL(disconnected()), this, SLOT(onClientUpdateStatus()));
    connect(m_pLocalClient, SIGNAL(readyRead()), this, SLOT(onClientReadMessage()));
}

void MainWindow::onClientUpdateStatus()
{
    if(NULL == m_pLocalClient) return;
    //disconnect(m_pLocalClient, SIGNAL(disconnected()), this, SLOT(onClientUpdateStatus()));
    //disconnect(m_pLocalClient, SIGNAL(readyRead()), this, SLOT(onClientReadMessage()));
    m_pLocalClient = NULL;
}

void MainWindow::onClientReadMessage()
{
    if(NULL == m_pLocalClient) return;
    QByteArray data = m_pLocalClient->readAll();
}

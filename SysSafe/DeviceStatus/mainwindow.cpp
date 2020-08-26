#include <QDateTime>
#include <QDebug>
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
    m_pTimerDevice->start(60*1000);
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

    connect(m_pQuitAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(m_pShowAction, SIGNAL(triggered()), this, SLOT(onShowWindow()));
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
    m_bInit = true;
    return true;
}

void MainWindow::onTimeOut()
{
    static unsigned int nTimeLast = 0;

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
    }
}

void MainWindow::onShowWindow()
{

}

void MainWindow::onCloseWindow()
{

}

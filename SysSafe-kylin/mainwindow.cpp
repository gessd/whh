#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGui/QCloseEvent>
#include <QtCore/QDateTime>
#include <QtCore/QModelIndex>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtWidgets/QApplication>
#include "xmessagebox.h"
#include "styledefine.h"
#include "setconfig.h"
#include <dlfcn.h>

#include <QDBusArgument>
#include <unistd.h>
#include <pwd.h>

#define	XG_ERR_SUCCESS                0x00
typedef char *LPCTSTR;

typedef struct _API_FV
{
    long (*FV_ConnectDev) (LPCTSTR sDev, LPCTSTR sPasswrod);
    long (*FV_CloseDev) (long lDevHandle);
    long (*FV_SendCmdPacket) (long lDevHandle, long lCmd, LPCTSTR sData);
    long (*FV_RecvCmdPacket) (long lDevHandle, LPCTSTR sData, long lTimeout);

} FVAPI_t, *pFVAPI_t;
static FVAPI_t m_FVAPI;
static long s_devHd = 0;
void *pSoHandle = NULL;

static void* GetFVAPI(void *pSoHandle, const char* sApiName)
{
    char* sError = NULL;
    void* pApi = NULL;

    if(pSoHandle == NULL || sApiName == NULL) return NULL;

    pApi = dlsym(pSoHandle, sApiName);
    sError = dlerror();
    if(sError != NULL)
    {
        //bio_print_error("API:%s ERROR:%s\n", sApiName, sError);
        return NULL;
    }
    return pApi;
}

static int InitSo(char* sSoFileName)
{
    char* sError = NULL;

    if(sSoFileName == NULL) return -1;

    memset(&m_FVAPI, 0, sizeof(FVAPI_t));

    pSoHandle = dlopen(sSoFileName, RTLD_LAZY);
    if(pSoHandle == NULL)
    {
        sError = dlerror();
        //bio_print_error("SO:%s ERROR:%s\n", sSoFileName, sError);
        return -1;
    }

    m_FVAPI.FV_ConnectDev = (long(*)(LPCTSTR,LPCTSTR))GetFVAPI(pSoHandle, "FV_ConnectDev");
    m_FVAPI.FV_CloseDev = (long(*)(long))GetFVAPI(pSoHandle, "FV_CloseDev");
    m_FVAPI.FV_SendCmdPacket = (long(*)(long,long,LPCTSTR))GetFVAPI(pSoHandle, "FV_SendCmdPacket");
    m_FVAPI.FV_RecvCmdPacket = (long(*)(long,LPCTSTR,long))GetFVAPI(pSoHandle, "FV_RecvCmdPacket");
}

enum EnWidgetIndex
{
    EnMainWidgetIndex=0,
    EnCreateUserWidgetIndex,
    EnSysSetWidgetIndex
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //暂时不需要菜单栏及状态栏
    ui->btnTitleSet->setVisible(false);
    m_pCurrenFingerButton = NULL;
    //去掉最大化与最小号窗口
    setWindowFlags(Qt::WindowMinMaxButtonsHint);
    //去掉边框
    setWindowFlags(Qt::FramelessWindowHint);
    //背景透明度
    setWindowOpacity(1);
    //设置背景透明
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_StyledBackground);
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
    ui->stackedWidget->setCurrentIndex(0);

    connect(m_pQuitAction, SIGNAL(triggered()), this, SLOT(onCloseWindow()));
    connect(m_pShowAction, SIGNAL(triggered()), this, SLOT(onShowWindow()));

    connect(m_pTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(onIconActivated(QSystemTrayIcon::ActivationReason)));
    connect(ui->btnWindowClose, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->btnTitleSet, SIGNAL(clicked()), this, SLOT(onBtnTitleSet()));
    connect(ui->btnTitleHelp, SIGNAL(clicked()), this, SLOT(onBtnTitleHelp()));
    connect(ui->btnTitleAbout, SIGNAL(clicked()), this, SLOT(onBtnTitleAbout()));
    connect(ui->btnSetBack, SIGNAL(clicked()), this, SLOT(onBtnBackMainClicked()));
    connect(ui->btnFingerBack, SIGNAL(clicked()), this, SLOT(onBtnBackMainClicked()));
    connect(ui->stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(onMainWidgetCurrentChanged(int)));
    connect(ui->btnFingerRemove, SIGNAL(clicked()), this, SLOT(onBtnFingerRemoveClicked()));
    connect(ui->btnAddVein, SIGNAL(clicked()), this, SLOT(onBtnAddVeinClicked()));
    connect(ui->btnFingerCheck, SIGNAL(clicked()), this, SLOT(onBtnFingerChecked()));
    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onItemClicked(QListWidgetItem*)));

    m_pGroupLanguage = new QButtonGroup(this);
    m_pGroupLanguage->addButton(ui->radioButton_zh_cn, 0);
    m_pGroupLanguage->addButton(ui->radioButton_zh_tw, 1);
    m_pGroupLanguage->addButton(ui->radioButton_eng, 2);
    m_pGroupLanguage->addButton(ui->radioButton_mw, 3);
    connect(m_pGroupLanguage, SIGNAL(buttonClicked(int)), this, SLOT(onSetLanguageClicked(int)));

    ui->progressBarFinger->setMaximum(_MaxProgressBar);
    m_pMovieFinger = new QMovie(":/images/fingermovie.gif");
    ui->labelMovieFinger->setMovie(m_pMovieFinger);
    m_pMovieFinger->start();

    installEventFilter(this);
    //使用事件过滤器出来界面移动
    ui->widgetTitle->installEventFilter(this);

    //加载界面样式
    QFile qss(":/qss/windowstyle.qss");
    if(qss.open(QFile::ReadOnly)){
        QString qstrQss = qss.readAll();
        qDebug()<<qstrQss;
        setStyleSheet(qstrQss);
        qss.close();
    }
    //kylin
    m_pFingerVeinDeviceInfo = NULL;
    /* 连接 DBus Daemon */
    serviceInterface = new QDBusInterface(DBUS_SERVICE, DBUS_PATH,
                                          DBUS_INTERFACE,
                                          QDBusConnection::systemBus());
    serviceInterface->setTimeout(2147483647); /* 微秒 */
    connect(serviceInterface, SIGNAL(USBDeviceHotPlug(int, int, int)), this, SLOT(onUSBDeviceHotPlug(int,int,int)));
    connect(serviceInterface, SIGNAL(StatusChanged(int,int)), this, SLOT(onStatusChanged(int,int)));

    ui->stackedWidgetSetSys->setCurrentIndex(0);
    int nRow = ui->listWidget->count();
    for(int i=0;i<nRow;i++){
        QListWidgetItem* item = ui->listWidget->item(i);
        if(NULL == item) continue;
        if(0 == i){
            item->setIcon(QIcon(":/images/action.png"));
        } else if(1 == i){
            item->setIcon(QIcon(":/images/account.png"));
        } else if(2 == i){
            item->setIcon(QIcon(":/images/language.png"));
        } else if(3 == i) {
            item->setIcon(QIcon(":/images/voice.png"));
        }
    }
#ifndef Q_OS_WIN

#else
    ui->labelDeviceStatus->setVisible(fasle);
#endif
    //注册自定义数据类型
    registerCustomTypes();

    ui->labelDeviceStatus->setVisible(false);
    bindButtonFinger(ui->btnFinger_1, _FingerStartIndex+1, ui->btnBlood_1);
    bindButtonFinger(ui->btnFinger_2, _FingerStartIndex+2, ui->btnBlood_2);
    bindButtonFinger(ui->btnFinger_3, _FingerStartIndex+3, ui->btnBlood_3);
    bindButtonFinger(ui->btnFinger_4, _FingerStartIndex+4, ui->btnBlood_4);
    bindButtonFinger(ui->btnFinger_5, _FingerStartIndex+5, ui->btnBlood_5);
    bindButtonFinger(ui->btnFinger_6, _FingerStartIndex+6, ui->btnBlood_6);
    bindButtonFinger(ui->btnFinger_7, _FingerStartIndex+7, ui->btnBlood_7);
    bindButtonFinger(ui->btnFinger_8, _FingerStartIndex+8, ui->btnBlood_8);
    bindButtonFinger(ui->btnFinger_9, _FingerStartIndex+9, ui->btnBlood_9);
    bindButtonFinger(ui->btnFinger_10, _FingerStartIndex+10, ui->btnBlood_10);
    ui->labelDeviceStatus->setText(QString("<font color=%1>%2</font>").arg(SetConfig::getSetValue(_MessageErrorColor, "#FF0000")).arg(ui->labelDeviceStatus->text()));

    connect(qApp, SIGNAL(messageReceived(QString)), this, SLOT(onMessageReceived(QString)));

    InitSo("/usr/lib/libXGComApi.so");

    //声音事件
    unsigned int nVoice = SetConfig::getSetValue(_VoiceSet, 1).toUInt();
    connect(ui->horizontalSliderVoice, SIGNAL(valueChanged(int)), this, SLOT(onVoiceValueChanged(int)));
    ui->horizontalSliderVoice->setValue(nVoice);

    m_nDBUSProgress = 0;
    ops = IDLE;
}

MainWindow::~MainWindow()
{
    uninitFingerData();
    delete ui;
}

int MainWindow::sysInit()
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
        return -1;
    }

    /* 解析 DBus 返回值，reply 有两个返回值，都是 QVariant 类型 */
    variant = reply.argumentAt(0); /* 得到第一个返回值 */
    deviceCount = variant.value<int>(); /* 解封装得到设备个数 */
    variant = reply.argumentAt(1); /* 得到第二个返回值 */
    argument = variant.value<QDBusArgument>(); /* 解封装，获取QDBusArgument对象 */
    argument >> qlist; /* 使用运算符重载提取 argument 对象里面存储的列表对象 */

    m_pFingerVeinDeviceInfo = NULL;
    for (int i = 0; i < deviceCount; i++) {
        item = qlist[i]; /* 取出一个元素 */
        variant = item.variant(); /* 转为普通QVariant对象 */
        /* 解封装得到 QDBusArgument 对象 */
        argument = variant.value<QDBusArgument>();
        deviceInfo = new DeviceInfo();
        argument >> *deviceInfo; /* 提取最终的 DeviceInfo 结构体 */

        if(BIOTYPE_FINGERVEIN == deviceInfo->biotype){
            if(deviceInfo->device_shortname.contains(_DeviceName_)){
                qDebug()<<"---device "<<i<<deviceInfo<<deviceInfo->device_id<<
                          deviceInfo->device_shortname<<deviceInfo->device_fullname<<deviceInfo->biotype;
                m_pFingerVeinDeviceInfo = deviceInfo;
                m_nDeviceId = m_pFingerVeinDeviceInfo->device_id;
            }
        }
    }
    ui->labelDeviceStatus->setVisible(false);
    ui->stackedWidget->widget(EnMainWidgetIndex)->setEnabled(true);
    //无设备或设备被禁用
    if(NULL == m_pFingerVeinDeviceInfo || m_pFingerVeinDeviceInfo->driver_enable <= 0 || m_pFingerVeinDeviceInfo->device_available <=0) {
        ui->labelDeviceStatus->setVisible(true);
        ui->stackedWidget->widget(EnMainWidgetIndex)->setEnabled(false);
        ui->stackedWidget->setCurrentIndex(EnMainWidgetIndex);
        return -1;
    }
    return 0;
}

void MainWindow::sysUnInit()
{
}

void MainWindow::showFingerInfo()
{
    ui->btnTitleSet->setVisible(true);
    ui->stackedWidget->setCurrentIndex(EnMainWidgetIndex);

    //获取已录入指静脉信息
    int drvId = m_nDeviceId;
    int uid = getuid();
    int idxStart = 0;
    int idxEnd = -1;
    QList<QVariant> args;
    args << drvId<< uid << idxStart << idxEnd;

    serviceInterface->callWithCallback("GetFeatureList", args, this,
                                       SLOT(showFeaturesCallback(QDBusMessage)),
                                       SLOT(errorCallback(QDBusError)));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_N){
    }else if(event->key() == Qt::Key_U){
    }
}

//在此出来鼠标拖动窗体移动
bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->widgetTitle) {
        //标题栏事件，用于拖动窗体
        static bool bMousePressed = false;
        static QPoint pointMouse = QPoint(0,0);
        if (event->type() == QEvent::MouseButtonPress) {
            //鼠标点击
            QMouseEvent *mouseEvent = (QMouseEvent *)event;
            bMousePressed = true;
            pointMouse = mouseEvent->globalPos() - pos();
            return true;
        }
        else if (event->type() == QEvent::MouseButtonRelease) {
            //鼠标释放
            bMousePressed = false;
            return true;
        }
        else if (event->type() == QEvent::MouseMove) {
            //鼠标移动
            QMouseEvent *mouseEvent = (QMouseEvent *)event;
            if (bMousePressed) {
                move(mouseEvent->globalPos() - pointMouse);
            }
            return true;
        }
        return false;
    } else if(this == watched){
        //监控程序是否有操作
        return false;
    }
    return false;
}

void MainWindow::onShowWindow()
{
    showNormal();
}

void MainWindow::onCloseWindow()
{
    qDebug()<<"准备关闭程序";
    uninitFingerData();
    QApplication::quit();
    qDebug()<<"程序关闭";
}

void MainWindow::onIconActivated(QSystemTrayIcon::ActivationReason reason)
{

}

void MainWindow::onTimeOut()
{
    QString qstrCurTime = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString qstrTitle = tr("安全登录");
    QString qstrMessage = tr("设备不在线，将执行安全策略");//+qstrCurTime;
    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Warning);
    QIcon ion = windowIcon();
    m_pTrayIcon->showMessage(qstrTitle, qstrMessage, icon, 1);
}

void MainWindow::onBtnTitleSet()
{
    int nLanguSet = SetConfig::getSetValue(_LanguageSet, 0).toInt();
    QAbstractButton* pBtnLang = m_pGroupLanguage->button(nLanguSet);
    if(pBtnLang) pBtnLang->setChecked(true);
    ui->stackedWidget->setCurrentIndex(EnSysSetWidgetIndex);
}

void MainWindow::onBtnTitleHelp()
{

}

void MainWindow::onBtnTitleAbout()
{
    XMessageBox::message(this, tr("关于我们"), tr("东方创芯（北京）数字技术有限公司\n版本：V1.1"));
}

void MainWindow::onBtnBackMainClicked()
{
    ui->stackedWidget->setCurrentIndex(EnMainWidgetIndex);
}

void MainWindow::onBtnSetUserPass()
{

}

void MainWindow::onMainWidgetCurrentChanged(int index)
{
    ui->btnTitleSet->setVisible(false);
    if(EnMainWidgetIndex == index) {
        //主窗口，显示设置按钮
        ui->btnTitleSet->setVisible(true);
    }
}

void MainWindow::onBtnLogonClicked()
{

}

void MainWindow::onItemClicked(QListWidgetItem *item)
{
    QModelIndex modeIndex = ui->listWidget->currentIndex();
    unsigned int nIndex = modeIndex.row();
    ui->stackedWidgetSetSys->setCurrentIndex(nIndex);
}

//主页手指点击
void MainWindow::onBtnFingerClicked()
{
    QObject* pObj = sender();
    if(NULL == pObj) {
        qDebug()<<"内部错误，手指点击处理没有处理对象.";
        return;
    }
    QToolButton* pButton = dynamic_cast<QToolButton*>(pObj);
    if(NULL == pButton) {
        qDebug()<<"内部错误，手指点击处理类型错误.";
        return;
    }

    bool isFinger = pButton->property(_FingerButtonIsInfo).toBool();
    if(isFinger){
        //有指静脉信息，展示验证界面
        ui->labelFingerText->clear();
        showVeinCompareWidget(pButton);
    } else {
        //无指静脉信息，展示录入信息界面
        ui->labelFingerText->clear();
        showVeinAddWidget(pButton);
    }
}

void MainWindow::onBtnAddVeinClicked()
{
    ui->stackedWidget->widget(EnCreateUserWidgetIndex)->setEnabled(false);
    int drvId = m_nDeviceId;
    int uid = getuid();
    int idx = m_pCurrenFingerButton->property(_ButtonFingerIndex).toInt();
    QString idxName = QString::number(idx);
    QList<QVariant> args;
    args << drvId << uid << idx << idxName;
    bool bCall = serviceInterface->callWithCallback("Enroll", args, this,
                                                    SLOT(enrollCallBack(const QDBusMessage &)),
                                                    SLOT(errorCallBack(const QDBusError &)));
    if(false == bCall) {
        ui->labelFingerText->setText(QString("<font color=%1>%2</font>").arg(SetConfig::getSetValue(_MessageErrorColor, "#FF0000")).arg(tr("录入操作错误")));
        ui->stackedWidget->widget(EnCreateUserWidgetIndex)->setEnabled(true);
        return;
    }
    m_nDBUSProgress = 1;
	ops = ENROLL;
    _FingerProgress(_MaxProgressBar*0.1);
    ui->labelFingerText->setText(tr("正在录入信息"));
}

void MainWindow::onBtnFingerRemoveClicked()
{
    ui->stackedWidget->widget(EnCreateUserWidgetIndex)->setEnabled(false);
    int drvId = m_nDeviceId;
    int uid = getuid();
    int idxStart = m_pCurrenFingerButton->property(_ButtonFingerIndex).toInt();
    int idxEnd = -1;
    _FingerProgress(_MaxProgressBar*0.1);
    QDBusPendingReply<int> reply = serviceInterface->call("Clean", drvId, uid, idxStart, idxEnd);
    reply.waitForFinished();
    if (reply.isError()) {
        ui->labelFingerText->setText(QString("<font color=%1>%2</font>").arg(SetConfig::getSetValue(_MessageErrorColor, "#FF0000")).arg(tr("删除失败")+" "+reply.error().message()));
        ui->stackedWidget->widget(EnCreateUserWidgetIndex)->setEnabled(true);
        return;
    }
    ui->labelFingerText->setText(tr("信息删除成功"));
    _FingerProgress(_MaxProgressBar*1);
    setButtonFingerInfo(m_pCurrenFingerButton, false);
    showVeinAddWidget(m_pCurrenFingerButton);
    ui->stackedWidget->widget(EnCreateUserWidgetIndex)->setEnabled(true);
}

//信息验证
void MainWindow::onBtnFingerChecked()
{
    ui->stackedWidget->widget(EnCreateUserWidgetIndex)->setEnabled(false);
    int drvId = m_nDeviceId;
    int uid = getuid();
    int idx = m_pCurrenFingerButton->property(_ButtonFingerIndex).toInt();
    QList<QVariant> args;
    args << drvId << uid << idx;

    bool bCall = serviceInterface->callWithCallback("Verify", args, this,
                                                    SLOT(verifyCallBack(const QDBusMessage &)),
                                                    SLOT(errorCallBack(const QDBusError &)));
    if(false == bCall) {
        ui->labelFingerText->setText(QString("<font color=%1>%2</font>").arg(SetConfig::getSetValue(_MessageErrorColor, "#FF0000")).arg(tr("验证操作错误")));
        ui->stackedWidget->widget(EnCreateUserWidgetIndex)->setEnabled(true);
        return;
    }
    _FingerProgress(_MaxProgressBar*0.1);
    ui->labelFingerText->setText(tr("正在验证信息"));
}

void MainWindow::onSetActionClicked(int buttonId)
{
    SetConfig::setSetValue(_ActionSet, buttonId);
}

void MainWindow::onSetLanguageClicked(int buttonId)
{
    SetConfig::setSetValue(_LanguageSet, buttonId);
    XMessageBox::StandardButton standard;
    standard = XMessageBox::question(this, tr("重启"), tr("修改语言后需要重启生效，是否现在重启？"));
    if (standard == XMessageBox::No) return;
    //重启程序
    QApplication::exit(RETCODE_RESTART);
}

void MainWindow::onTimeOutOperation()
{

}

void MainWindow::onTimeOutDeviceOffline()
{

}

QToolButton *MainWindow::getFingerButton(int index)
{
    if(index == ui->btnFinger_1->property(_ButtonFingerIndex).toInt()){
        return ui->btnFinger_1;
    } else if(index == ui->btnFinger_2->property(_ButtonFingerIndex).toInt()){
        return ui->btnFinger_2;
    } else if(index == ui->btnFinger_3->property(_ButtonFingerIndex).toInt()){
        return ui->btnFinger_3;
    } else if(index == ui->btnFinger_4->property(_ButtonFingerIndex).toInt()){
        return ui->btnFinger_4;
    } else if(index == ui->btnFinger_5->property(_ButtonFingerIndex).toInt()){
        return ui->btnFinger_5;
    } else if(index == ui->btnFinger_6->property(_ButtonFingerIndex).toInt()){
        return ui->btnFinger_6;
    } else if(index == ui->btnFinger_7->property(_ButtonFingerIndex).toInt()){
        return ui->btnFinger_7;
    } else if(index == ui->btnFinger_8->property(_ButtonFingerIndex).toInt()){
        return ui->btnFinger_8;
    } else if(index == ui->btnFinger_9->property(_ButtonFingerIndex).toInt()){
        return ui->btnFinger_9;
    } else if(index == ui->btnFinger_10->property(_ButtonFingerIndex).toInt()){
        return ui->btnFinger_10;
    }
    return NULL;
}

void MainWindow::onUSBDeviceHotPlug(int drvid, int action, int devNumNow)
{
    //if(_DeviceId_ != drvid) return;
    sysInit();
    showFingerInfo();
    /*
    if(action <=0 || devNumNow <= 0){
        ui->labelDeviceStatus->setVisible(true);
        ui->stackedWidget->widget(EnMainWidgetIndex)->setEnabled(false);
        ui->stackedWidget->setCurrentIndex(EnMainWidgetIndex);
    } else {
        ui->labelDeviceStatus->setVisible(false);
        ui->stackedWidget->widget(EnMainWidgetIndex)->setEnabled(true);
    }
    */
}

int setSound(int Sound)
{
    if(!m_FVAPI.FV_ConnectDev) return -1;
    s_devHd = m_FVAPI.FV_ConnectDev((char*)"VID=8455,PID=30008", (char*)"00000000");
    if (s_devHd <= 0) {
        //bio_print_error("df100_ops_open failed\n");
        s_devHd = 0;
        return -1;
    }
    if (s_devHd <= 0) {
        return -1;
    }
    int ret;
    unsigned char bData[16] = { 0 };
    char sData[50] = { 0 };

    sprintf(sData, "20%02X", Sound);
    ret = m_FVAPI.FV_SendCmdPacket(s_devHd, 0x4B, sData);
    if(ret == XG_ERR_SUCCESS)
    {
        m_FVAPI.FV_RecvCmdPacket(s_devHd, sData, 1000);
    }
    if (s_devHd > 0) {
        m_FVAPI.FV_CloseDev(s_devHd);
        s_devHd = 0;
    }
    return ret*-1;
}

void MainWindow::onVoiceValueChanged(int value)
{
    setSound(value);
    if(0 > value) value = 0;
    if(value > 15) value = 15;
    ui->labelVoiceNum->setText(tr("音量:")+ QString::number(value));
}

void MainWindow::onMessageReceived(QString qstrMessage)
{
    onShowWindow();
}

void MainWindow::onStatusChanged(int drvId, int statusType)
{
    if(m_nDeviceId != drvId) return;

    if (ops == ENROLL) {
	    if(m_nDBUSProgress>0) {
		    _FingerProgress((m_nDBUSProgress-1)*33);
		    m_nDBUSProgress++;
	    }
    }
    QDBusMessage notifyReply = serviceInterface->call("GetNotifyMesg", drvId);
    if(notifyReply.type() == QDBusMessage::ErrorMessage) {
        qDebug() << "DBUS: " << notifyReply.errorMessage();
        return;
    }
    QString prompt = notifyReply.arguments().at(0).toString();
    qDebug() << "---- message:"<<prompt;
    ui->labelFingerText->setText(prompt);
}

void MainWindow::enrollCallBack(const QDBusMessage &reply)
{
    int result;
    result = reply.arguments()[0].value<int>();
    qDebug() << "Enroll result: " << result;
    switch(result) {
    case DBUS_RESULT_SUCCESS: { /* 录入成功 */
        ui->labelFingerText->setText(tr("录入成功"));
        _FingerProgress(1*_MaxProgressBar);
        showVeinCompareWidget(m_pCurrenFingerButton);
        setButtonFingerInfo(m_pCurrenFingerButton, true);
        break;
    }
    default:
        ui->labelFingerText->setText(QString("<font color=%1>%2</font>").arg(SetConfig::getSetValue(_MessageErrorColor, "#FF0000")).arg(tr("录入失败")));
        break;
    }
    ui->stackedWidget->widget(EnCreateUserWidgetIndex)->setEnabled(true);
    ops = IDLE;
}

void MainWindow::verifyCallBack(const QDBusMessage &reply)
{
    int result;
    result = reply.arguments()[0].value<int>();

    if(result >= 0) {
        ui->labelFingerText->setText(tr("验证成功"));
        _FingerProgress(1*_MaxProgressBar);
    } else if(result == DBUS_RESULT_NOTMATCH) {
        ui->labelFingerText->setText(QString("<font color=%1>%2</font>").arg(SetConfig::getSetValue(_MessageErrorColor, "#FF0000")).arg(tr("验证失败，不匹配")));
        _FingerProgress(0.5*_MaxProgressBar);
    } else {
        ui->labelFingerText->setText(QString("<font color=%1>%2</font>").arg(SetConfig::getSetValue(_MessageErrorColor, "#FF0000")).arg(tr("验证失败")+" "+handleErrorResult(result)));
        _FingerProgress(0.5*_MaxProgressBar);
    }
    ui->stackedWidget->widget(EnCreateUserWidgetIndex)->setEnabled(true);
}

void MainWindow::searchCallBack(const QDBusMessage &reply)
{

}

void MainWindow::StopOpsCallBack(const QDBusMessage &reply)
{

}

void MainWindow::errorCallBack(const QDBusError &error)
{
    qDebug() << "DBus Error: " << error.message();
    QString qstrErrorMessage = error.message();
    if(!qstrErrorMessage.isEmpty()){
        ui->labelFingerText->setText(QString("<font color=%1>%2</font>").arg(SetConfig::getSetValue(_MessageErrorColor, "#FF0000")).arg(qstrErrorMessage));
    }
    ui->stackedWidget->widget(EnCreateUserWidgetIndex)->setEnabled(true);
}

void MainWindow::errorCallback(QDBusError error)
{
    qDebug() << "DBus Error: " << error.message();
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
        QString qstrInfo = QString("---- uid:%1 biotype:%2 device_shortname:%3 index:%4 index_name:%5")
                .arg(featureInfo->uid).arg(featureInfo->biotype).arg(featureInfo->device_shortname).arg(featureInfo->index).arg(featureInfo->index_name);

        //指静脉信息绑定到界面按钮
        QToolButton* pCurrentButton = getFingerButton(featureInfo->index);
        if(pCurrentButton){
            setButtonFingerInfo(pCurrentButton, true);
        } else {
            setButtonFingerInfo(pCurrentButton, false);
        }
    }
}

void MainWindow::initFingerData(QToolButton *button, int index)
{
    connect(button, SIGNAL(clicked()), this, SLOT(onBtnFingerClicked()));
}

void MainWindow::uninitFingerData()
{

}

//绑定手指指静脉信息
//pBtnFinger 手指按钮
//index 指静脉信息id
//pBtnBlood 手指上图片
void MainWindow::bindButtonFinger(QToolButton* pBtnFinger, int index, QToolButton *pBtnBlood)
{
    if(pBtnFinger == NULL || pBtnBlood == NULL) return;
    pBtnFinger->setProperty(_ButtonFingerIndex, index);
    _BindFingerBloodButton(pBtnFinger, pBtnBlood);
    connect(pBtnFinger, SIGNAL(clicked()), this, SLOT(onBtnFingerClicked()));
}

//显示录入指静脉信息界面
void MainWindow::showVeinAddWidget(QToolButton *button)
{
    if(NULL == button) return;
    ui->btnFingerRemove->setVisible(false);
    ui->btnFingerCheck->setVisible(false);
    ui->btnAddVein->setVisible(true);
    _FingerProgress(0*_MaxProgressBar);
    ui->labelMessage->setText(tr("录入"));
    m_pCurrenFingerButton = button;
    ui->stackedWidget->setCurrentIndex(EnCreateUserWidgetIndex);
}

//显示验证指静脉信息界面
void MainWindow::showVeinCompareWidget(QToolButton *button)
{
    if(NULL == button) return;
    ui->btnFingerRemove->setVisible(true);
    ui->btnFingerCheck->setVisible(true);
    ui->btnAddVein->setVisible(false);
    _FingerProgress(0*_MaxProgressBar);
    ui->labelMessage->setText(tr("验证"));
    m_pCurrenFingerButton = button;
    ui->stackedWidget->setCurrentIndex(EnCreateUserWidgetIndex);
}

QString MainWindow::handleErrorResult(int error)
{
    switch(error) {
    case DBUS_RESULT_ERROR: {
        //操作失败，需要进一步获取失败原因
        //QDBusMessage msg = serviceInterface->call("GetOpsMesg", deviceId);
        //if(msg.type() == QDBusMessage::ErrorMessage){
        //qDebug() << "UpdateStatus error: " << msg.errorMessage();
        //return "接口错误";
        //}
        break;
    }
    case DBUS_RESULT_DEVICEBUSY:
        return "设备忙";
        break;
    case DBUS_RESULT_NOSUCHDEVICE:
        return tr("设备不存在");
        break;
    case DBUS_RESULT_PERMISSIONDENIED:
        return tr("没有权限");
        break;
    }
    return tr("未知错误");
}

void MainWindow::setButtonFingerInfo(QToolButton* pButton, bool bInfo)
{
    if(NULL == pButton) return;
    pButton->setProperty(_FingerButtonIsInfo, bInfo);
    QToolButton* pBtnBlood = pButton->property(_ButtonPropertyName).value<QToolButton*>();
    if(pBtnBlood) pBtnBlood->setVisible(bInfo);
    if(bInfo){
        pButton->setStyleSheet(_FingerStyleButton);
    } else {
        pButton->setStyleSheet(_ButtonFingerNoDataStytle);
    }
}

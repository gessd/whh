#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGui/QCloseEvent>
#include <QtCore/QDateTime>
#include <QtCore/QModelIndex>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include "xmessagebox.h"
#include "styledefine.h"
#include "setconfig.h"

enum EnWidgetIndex
{
    EnLoginWidgetIndex = 0,
    EnMainWidgetIndex,
    EnCreateUserWidgetIndex,
    EnSysSetWidgetIndex
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_pTrayIcon = NULL;
    m_pTrayIconMenu = NULL;
    m_pQuitAction = NULL;
    m_pShowAction = NULL;
    m_pTimer = NULL;

    //暂时不需要菜单栏及状态栏
    ui->btnTitleSet->setVisible(false);
    ui->labelPassMassage->clear();

    //setWindowFlags();
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
    m_pQuitAction = new QAction(tr("退出"), this);
    m_pShowAction = new QAction(tr("显示"), this);

    //创建状态栏
    m_pTrayIconMenu = new QMenu(this);
    m_pTrayIconMenu->addAction(m_pShowAction);
    m_pTrayIconMenu->addSeparator();
    m_pTrayIconMenu->addAction(m_pQuitAction);
    m_pTrayIcon = new QSystemTrayIcon(this);
    m_pTrayIcon->setContextMenu(m_pTrayIconMenu);

    m_pCurrenFingerButton = NULL;

    QIcon icon(QIcon(":/images/heart.png"));
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
    connect(ui->btnLogon, SIGNAL(clicked()), this, SLOT(onBtnLogonClicked()));
    connect(ui->lineEditPass, SIGNAL(returnPressed()), this, SLOT(onBtnLogonClicked()));
    connect(ui->btnSetPass, SIGNAL(clicked()), this, SLOT(onBtnSetUserPass()));
    connect(ui->btnFingerRemove, SIGNAL(clicked()), this, SLOT(onBtnFingerRemoveClicked()));
    connect(ui->btnAddVein, SIGNAL(clicked()), this, SLOT(onBtnAddVeinClicked()));
    connect(ui->btnFingerCheck, SIGNAL(clicked()), this, SLOT(onBtnFingerChecked()));
    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onItemClicked(QListWidgetItem*)));
    initFingerData(ui->btnFinger_1, 1);
    initFingerData(ui->btnFinger_2, 2);
    initFingerData(ui->btnFinger_3, 3);
    initFingerData(ui->btnFinger_4, 4);
    initFingerData(ui->btnFinger_5, 5);
    initFingerData(ui->btnFinger_6, 6);
    initFingerData(ui->btnFinger_7, 7);
    initFingerData(ui->btnFinger_8, 8);
    initFingerData(ui->btnFinger_9, 9);
    initFingerData(ui->btnFinger_10, 10);
    _BindFingerBloodButton(ui->btnFinger_1, ui->btnBlood_1);
    _BindFingerBloodButton(ui->btnFinger_2, ui->btnBlood_2);
    _BindFingerBloodButton(ui->btnFinger_3, ui->btnBlood_3);
    _BindFingerBloodButton(ui->btnFinger_4, ui->btnBlood_4);
    _BindFingerBloodButton(ui->btnFinger_5, ui->btnBlood_5);
    _BindFingerBloodButton(ui->btnFinger_6, ui->btnBlood_6);
    _BindFingerBloodButton(ui->btnFinger_7, ui->btnBlood_7);
    _BindFingerBloodButton(ui->btnFinger_8, ui->btnBlood_8);
    _BindFingerBloodButton(ui->btnFinger_9, ui->btnBlood_9);
    _BindFingerBloodButton(ui->btnFinger_10, ui->btnBlood_10);

    m_pGroupAction = new QButtonGroup(this);
    m_pGroupAction->addButton(ui->radioButton_none, 0);
    m_pGroupAction->addButton(ui->radioButton_lockScreen, 1);
    m_pGroupAction->addButton(ui->radioButton_logout, 2);
    m_pGroupAction->addButton(ui->radioButton_reboot, 3);
    m_pGroupAction->addButton(ui->radioButton_halt, 4);
    connect(m_pGroupAction, SIGNAL(buttonClicked(int)), this, SLOT(onSetActionClicked(int)));

    m_pGroupLanguage = new QButtonGroup(this);
    m_pGroupLanguage->addButton(ui->radioButton_zh_cn, 0);
    m_pGroupLanguage->addButton(ui->radioButton_zh_tw, 1);
    m_pGroupLanguage->addButton(ui->radioButton_eng, 2);
    m_pGroupLanguage->addButton(ui->radioButton_mw, 3);
    connect(m_pGroupLanguage, SIGNAL(buttonClicked(int)), this, SLOT(onSetLanguageClicked(int)));

    ui->btnLogon->setVisible(false);
    ui->btnSetPass->setVisible(false);
    ui->progressBarFinger->setMaximum(_MaxProgressBar);
    m_pMovieFinger = new QMovie(":/images/fingermovie.gif");
    ui->labelMovieFinger->setMovie(m_pMovieFinger);
    m_pMovieFinger->start();

    m_pTimerOperation = new QTimer(this);
    m_pTimerOperation->setProperty("time", QVariant::fromValue(QDateTime::currentDateTime().toTime_t()));
    m_pTimerOperation->start(60*1000);
    connect(m_pTimerOperation, SIGNAL(timeout()), this, SLOT(onTimeOutOperation()));

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

    m_pTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(onTimeOut()));
    m_pTimer->start(60*1000);

    connect(&m_userManage, SIGNAL(timeOutDeviceOffline()), this, SLOT(onTimeOutDeviceOffline()));
}


MainWindow::~MainWindow()
{
    uninitFingerData();
    delete ui;
}

int MainWindow::sysInit()
{

    return m_userManage.QF_init();
}

void MainWindow::sysUnInit()
{
    m_userManage.QF_unInit();
}

void MainWindow::showUserName()
{
    WCHAR wUserName[256] = {0};
    int nGet = m_userManage.QF_getUserName(wUserName);
    if(0 > nGet){
        ui->labelPassMassage->setText(tr("获取用户信息失败，请重试"));
        return;
    }
    ui->btnLogon->setVisible(true);
    QString qwstrUserName = QString::fromStdWString(wUserName);
    ui->lineEditUserName->setText(qwstrUserName);

    int nCheck = m_userManage.QF_checkPwd();
    if(0 == nCheck){
        qDebug()<<"用户密码为空，需要重新设置密码"<<qwstrUserName;
        ui->btnSetPass->setVisible(true);
        onBtnSetUserPass();
    }
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
        switch (event->type()) {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
        case QEvent::MouseMove:
        case QEvent::KeyPress:
        case QEvent::KeyRelease:
        case QEvent::Enter:
        case QEvent::Leave:
            m_pTimerOperation->setProperty("time", QVariant::fromValue(QDateTime::currentDateTime().toTime_t()));
            break;
        default:
            break;
        }
        return false;
    }
    return false;
}

void MainWindow::onShowWindow()
{
    if(false == m_userManage.isInit()) return;
    showNormal();
}

void MainWindow::onCloseWindow()
{
    qDebug()<<"准备关闭程序";
    uninitFingerData();
    m_userManage.quitManage();
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
    int nActionSet = SetConfig::getSetValue(_ActionSet, 0).toInt();
    int nLanguSet = SetConfig::getSetValue(_LanguageSet, 0).toInt();
    QAbstractButton* pBtnAction = m_pGroupAction->button(nActionSet);
    if(pBtnAction) pBtnAction->setChecked(true);
    QAbstractButton* pBtnLang = m_pGroupLanguage->button(nLanguSet);
    if(pBtnLang) pBtnLang->setChecked(true);
    ui->stackedWidget->setCurrentIndex(EnSysSetWidgetIndex);
}

void MainWindow::onBtnTitleHelp()
{

}

void MainWindow::onBtnTitleAbout()
{

}

void MainWindow::onBtnBackMainClicked()
{
    ui->stackedWidget->setCurrentIndex(EnMainWidgetIndex);
}

void MainWindow::onBtnSetUserPass()
{
    int nc = m_userManage.QF_checkPwd();

    CreateUserDialog setUserDialog(this);
    setUserDialog.setUserName(ui->lineEditUserName->text().trimmed());
    if(QDialog::Accepted == setUserDialog.exec()){
        QString qstrPass = setUserDialog.getPassword();
        QString qstrUserName = ui->lineEditUserName->text().trimmed();
        std::wstring wstrUserName = qstrUserName.toStdWString();
        std::wstring wstrPass = qstrPass.toStdWString();
        int nSet = m_userManage.QF_setPassword(const_cast<WCHAR*>(wstrUserName.c_str()), const_cast<WCHAR*>(wstrPass.c_str()));
        if(0 > nSet){
            ui->labelPassMassage->setText(tr("密码设置错误"));
            onBtnSetUserPass();
        }
    }
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
    std::wstring wstrUserName = ui->lineEditUserName->text().trimmed().toStdWString();
    std::wstring wstrPass = ui->lineEditPass->text().trimmed().toStdWString();
    int nVeri = m_userManage.QF_veriPassword(const_cast<WCHAR*>(wstrUserName.c_str()), const_cast<WCHAR*>(wstrPass.c_str()));
    if(0 != nVeri){
        ui->labelPassMassage->setText(tr("登录失败: 未知的用户名或错误密码."));
        ui->lineEditPass->clear();
        return;
    }
#ifdef _USER_DRV_API_
    //获取用户信息及指静脉信息
    memset(m_stUserData.user_name, 0 ,sizeof(m_stUserData.user_name));
    memset(m_stUserData.passwd, 0 ,sizeof(m_stUserData.passwd));
    wcscpy(m_stUserData.user_name, wstrUserName.c_str());
    wcscpy(m_stUserData.passwd, wstrPass.c_str());
#endif
    qDebug()<<"更新用户信息";
    int nAdd = m_userManage.QF_userAdd(&m_stUserData);
    if(0 > nAdd){
        ui->labelPassMassage->setText(tr("用户信息创建失败，无法登陆"));
        qDebug()<<"添加用户失败"<<QString::fromStdWString(wstrUserName)<<QString::fromStdWString(wstrPass);
        return;
    }
    StUserEx* pUserEx = NULL;
#ifdef _USER_DRV_API_
    pUserEx = m_userManage.QF_getUserInfoByUserName(m_stUserData.user_name);
#endif
    if(NULL == pUserEx){
        ui->labelPassMassage->setText(tr("用户信息获取失败，无法登陆"));
        qDebug()<<"用户信息获取失败，无法登陆"<<QString::fromStdWString(wstrUserName)<<QString::fromStdWString(wstrPass);
        return;
    }
    StVeinEx* pVeinEx = pUserEx->vein;
    while (pVeinEx) {
        StVein vein;
        vein.fg_id = pVeinEx->fg_id;
        vein.null_id = pVeinEx->null_id;
        vein.user_id = pVeinEx->user_id;
        vein.vein_id = pVeinEx->vein_id;
#ifdef _USER_DRV_API_
        strcpy(vein.temp, pVeinEx->temp);
#endif
        QToolButton* pCurrentButton = bindButtonFinger(vein);
        if(NULL == pCurrentButton) {
            XMessageBox::warning(this, tr("警告"), tr("指静脉信息绑定错误，无法使用"));
            m_userManage.QF_freeUserInfo(pUserEx);
            return;
        } else {
            //当前按钮有指静脉信息
            pCurrentButton->setStyleSheet(_FingerStyleButton);
            QToolButton* pBloodButton = pCurrentButton->property(_ButtonPropertyName).value<QToolButton*>();
            if(pBloodButton) pBloodButton->setVisible(true);
        }
        pVeinEx = pVeinEx->next;
    }
    m_userManage.QF_freeUserInfo(pUserEx);

    //登录验证成功
    ui->stackedWidget->setCurrentIndex(EnMainWidgetIndex);
    ui->lineEditPass->clear();
    ui->labelPassMassage->clear();
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
    StVein* pVein = m_mapButtonFinger[pButton];
    if(NULL == pVein) {
        qDebug()<<"内部错误，手指按钮没有绑定数据.";
        return;
    }
    std::string wstrVeinData = pVein->temp;
    if(wstrVeinData.empty()){
        //无指静脉信息，展示录入信息界面
        ui->labelFingerText->clear();
        showVeinAddWidget(pButton);
    } else {
        //有指静脉信息，展示验证界面
        ui->labelFingerText->clear();
        showVeinCompareWidget(pButton);
    }
}

void MainWindow::onBtnAddVeinClicked()
{
    if(NULL == m_pCurrenFingerButton) return;
    StVein* pVein = m_mapButtonFinger[m_pCurrenFingerButton];
    if(NULL == pVein) return;
    pVein->user_id = m_stUserData.user_id;
    ui->labelFingerText->setText(tr("准备录入指静脉信息"));

    int nCon = m_userManage.QF_connectDev();
    if(0 != nCon){
        ui->labelFingerText->setText(tr("无法连接设备，请重试"));
        return;
    }
    ui->labelFingerText->setText(tr("设备连接成功，请放入手指"));
    QWidget* curWidget = ui->stackedWidget->currentWidget();
    if(curWidget){
        //设置当前页面无效，防止误操作
        curWidget->setEnabled(false);
    }

    QToolButton* pBloodButton = NULL;
    int nCheck = -1;
    const int nVienNum = 3;                               //需要录入三次
    const int nVeinDataLen = 1024*20;
    char cVeinData[nVienNum][nVeinDataLen] = {0};   //三次临时录入的指静脉信息
    char cVeinComplete[10*nVeinDataLen] = {0};      //合并后完整的指静脉信息
    float fVeinLen = -1;
    int nCompare = -1;
    int nCreate = -1;
    unsigned int nStartTime = QDateTime::currentDateTime().toTime_t();

    ui->labelFingerText->setText(tr("开始检测指静脉信息"));
    _FingerProgress(_MaxProgressBar*0.05);
    //录入时需要获取三遍指静脉特征然后融合后保存
    for(int i=0; i<nVienNum; i++){
        nCheck = m_userManage.QF_checkFinger(10, true);
        if(0 >= nCheck) {
            ui->labelFingerText->setText(tr("未检测到手指，请重新录入"));
            goto ToFingerDisCon;
        }
        fVeinLen = m_userManage.QF_getVeinChara(cVeinData[i]);
        if(0 >= fVeinLen) {
            ui->labelFingerText->setText(tr("未检测到指静脉信息，请重新录入"));
            goto ToFingerDisCon;
        }
        if(i>0){
            //与上一次对比是否为同一根手指,不是则重新录入
            nCompare = m_userManage.QF_veinCompare(cVeinData[i-1], cVeinData[i], 60);
            if(0 > nCompare){
                memset(cVeinData[i], 0, nVeinDataLen);
                i--;
                ui->labelFingerText->setText(tr("不是同一根手指，请重新录入"));
                goto ToFingerDisCon;
            }
        }
        _FingerProgress((i+1)*0.3*_MaxProgressBar);
        ui->labelFingerText->setText(QString(tr("第%1次检测完成，请重新放入手指")).arg(i+1));

        //timeout=-1 永远不超时
        nCheck = m_userManage.QF_checkFinger(-1, false);
        if(0 >= nCheck) {
            ui->labelFingerText->setText(tr("请重新放入手指"));
        }
    }
    ui->labelFingerText->setText(tr("指静脉信息录入完成，正在处理"));
    //三次指静脉信息录入完成后合并信息
    nCreate = m_userManage.QF_createVeinTemp(cVeinData[0], cVeinData[1], cVeinData[2], cVeinComplete);
    if(0 >= nCreate){
        _FingerProgress(0*_MaxProgressBar);
        ui->labelFingerText->setText(tr("指静脉信息处理失败，请重新录入"));
        goto ToFingerDisCon;
    }
    _FingerProgress(0.99*_MaxProgressBar);

    //登记指静脉信息
#ifdef _USER_DRV_API_
    strcpy(pVein->temp, cVeinComplete);
#endif
    nCreate = m_userManage.QF_veinAdd(pVein);
    if(0 > nCreate){
        _FingerProgress(0*_MaxProgressBar);
        ui->labelFingerText->setText(tr("指静脉信息登记失败，请重试"));
        goto ToFingerDisCon;
    }
    _FingerProgress(1*_MaxProgressBar);
    ui->labelFingerText->setText(tr("信息录入完成"));
    showVeinCompareWidget(m_pCurrenFingerButton);

    //录入完成主页对应血管图片
    pBloodButton = m_pCurrenFingerButton->property(_ButtonPropertyName).value<QToolButton*>();
    if(pBloodButton) pBloodButton->setVisible(true);
    m_pCurrenFingerButton->setStyleSheet(_FingerStyleButton);
ToFingerDisCon:
    int nDiscon = m_userManage.QF_disconnectDev();
    if(0 > nDiscon){
        QString qstrLast = ui->labelFingerText->text()+" ";
        ui->labelFingerText->setText(qstrLast+tr("错误，无法断开设备"));
    }
    curWidget->setEnabled(true);
}

void MainWindow::onBtnFingerRemoveClicked()
{
    if(NULL == m_pCurrenFingerButton) return;
    StVein* pVein = m_mapButtonFinger[m_pCurrenFingerButton];
    if(NULL == pVein) return;

    ui->labelFingerText->setText(tr("正在删除信息，请稍后"));
    int nDel = m_userManage.QF_veinDel(pVein);
    if(0 > nDel){
        ui->labelFingerText->setText(tr("指静脉信息删除失败，请重试"));
        return;
    }
#ifdef _USER_DRV_API_
    //清空成员变量中记录的信息
    memset(m_mapButtonFinger[m_pCurrenFingerButton]->temp, 0, sizeof(m_mapButtonFinger[m_pCurrenFingerButton]->temp));
#endif
    ui->labelFingerText->setText(tr("信息删除完成"));
    m_pCurrenFingerButton->setStyleSheet(_ButtonFingerNoDataStytle);
    QToolButton* pBloodButton = m_pCurrenFingerButton->property(_ButtonPropertyName).value<QToolButton*>();
    if(pBloodButton) pBloodButton->setVisible(false);
    showVeinAddWidget(m_pCurrenFingerButton);
}

//信息验证
void MainWindow::onBtnFingerChecked()
{
    if(NULL == m_pCurrenFingerButton) return;
    StVein* pVein = m_mapButtonFinger[m_pCurrenFingerButton];
    if(NULL == pVein) return;

    _FingerProgress(0*_MaxProgressBar);
    ui->labelFingerText->setText(tr("开始验证指静脉信息"));

    int nCon = m_userManage.QF_connectDev();
    if(0 != nCon){
        ui->labelFingerText->setText(tr("无法连接设备，请重试"));
        return;
    }
    QWidget* curWidget = ui->stackedWidget->currentWidget();
    if(curWidget){
        curWidget->setEnabled(false);
    }

    ui->labelFingerText->setText(tr("设备连接成功，请放入手指"));
    int nCheck = -1;
    const int nVeinDataLen = 1024*20;
    char cVeinData[nVeinDataLen] = {0};
    long lVeinLen = -1;
    int nCompare = -1;
    _FingerProgress(0.05*_MaxProgressBar);
    nCheck = m_userManage.QF_checkFinger(10, true);
    if(0 >= nCheck) {
        ui->labelFingerText->setText(tr("未检测到手指，请重试"));
        goto ToFingerDisCon;
    }
    //}
    _FingerProgress(0.5*_MaxProgressBar);
    ui->labelFingerText->setText(tr("正在检测指静脉信息，请稍后"));
    //等待检测指静脉信息
    lVeinLen = m_userManage.QF_getVeinChara(cVeinData);
    if(0>= lVeinLen){
        ui->labelFingerText->setText(tr("未检测到指静脉信息，请重试"));
        goto ToFingerDisCon;
    }
    //}
    _FingerProgress(0.9*_MaxProgressBar);
    ui->labelFingerText->setText(tr("正在验证指静脉信息，请稍后"));
    //验证信息
#ifdef _USER_DRV_API_
    nCompare = m_userManage.QF_veinVerifyUser(cVeinData, pVein->temp);
#endif
    if(0 != nCompare){
        ui->labelFingerText->setText(tr("指静脉信息不匹配，请重试"));
        goto ToFingerDisCon;
    }
    _FingerProgress(1*_MaxProgressBar);
    ui->labelFingerText->setText(tr("信息匹配一致"));
ToFingerDisCon:
    int nDiscon = m_userManage.QF_disconnectDev();
    if(0 != nDiscon){
        ui->labelFingerText->setText(tr("无法断开设备，请重试"));
        return;
    }
    if(curWidget){
        curWidget->setEnabled(true);
    }
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
    unsigned int nLastTime = m_pTimerOperation->property("time").toInt();
    unsigned int nCurrentTime = QDateTime::currentDateTime().toTime_t();

    if((nCurrentTime - nLastTime) < _NoActionMaxTime) return;
    int nAction = m_pGroupAction->checkedId();
    int nRes = -1;
    switch (nAction) {
    case 0: nRes = 0; break; //无动作
    case 1: nRes = m_userManage.QF_lockScreen();
        break;
    case 2:
        nRes = m_userManage.QF_logout();
        break;
    case 3: nRes = m_userManage.QF_reboot();
        break;
    case 4: nRes = m_userManage.QF_halt();
        break;
    default:
        break;
    }
    if(0> nRes){
        qDebug()<<"操作执行失败";
    }
}

void MainWindow::onTimeOutDeviceOffline()
{

}

void MainWindow::initFingerData(QToolButton *button, int index)
{
    StVein* vein = new StVein;
    vein->fg_id = index;
    vein->user_id = m_stUserData.user_id;
#ifdef _USER_DRV_API_
    memset(vein->temp, 0, sizeof(vein->temp));
#endif
    m_mapButtonFinger.insert(button, vein);
    connect(button, SIGNAL(clicked()), this, SLOT(onBtnFingerClicked()));
}

void MainWindow::uninitFingerData()
{
    QList<StVein*> list = m_mapButtonFinger.values();
    foreach(StVein* vein, list){
        if(NULL == vein) continue;
        delete vein;
        vein = NULL;
    }
    m_mapButtonFinger.clear();
}

QToolButton* MainWindow::bindButtonFinger(StVein vein)
{
    //通过map遍历一遍绑定的按钮与数据,根据id匹配
    QList<QToolButton*> listKeys = m_mapButtonFinger.keys();
    foreach (QToolButton* button, listKeys) {
        StVein* pVein = m_mapButtonFinger[button];
        if(NULL == pVein) continue;
        if(pVein->fg_id != vein.fg_id) continue;
        pVein->fg_id = vein.fg_id;
        pVein->user_id = vein.user_id;
        pVein->vein_id = vein.vein_id;
        pVein->null_id = vein.null_id;
#ifdef _USER_DRV_API_
        strcpy(pVein->temp, vein.temp);
#endif
        return button;
    }
    return NULL;
}

//显示录入指静脉信息界面
void MainWindow::showVeinAddWidget(QToolButton *button)
{
    if(NULL == button) return;
    ui->btnFingerRemove->setVisible(false);
    ui->btnFingerCheck->setVisible(false);
    ui->btnAddVein->setVisible(true);
    _FingerProgress(0*_MaxProgressBar);

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

    m_pCurrenFingerButton = button;
    ui->stackedWidget->setCurrentIndex(EnCreateUserWidgetIndex);
}

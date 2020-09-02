#include "usermanage.h"
#include <QtWidgets/QApplication>
#include <QtCore/QDebug>
#include <QtCore/QDateTime>
#include "styledefine.h"
#include "setconfig.h"

UserManage::UserManage(QObject *parent) : QObject(parent)
{
    m_bInit = false;
    m_bQuit = false;
    m_bDeviceConnect = false;
    m_pParam = new _StFunParamAndRes;
    m_pUserThread = new ContorllThread;
    m_pTimerCheckDevice = new QTimer(this);
    connect(m_pTimerCheckDevice, SIGNAL(timeout()), this, SLOT(onTimeOutCheckDecie()));
}

UserManage::~UserManage()
{
    if (m_pUserThread) {
        if (m_pUserThread->isRunning()) {
            m_pUserThread->quit();
        }
        delete m_pUserThread;
        m_pUserThread = NULL;
    }
    if (m_pParam) {
        delete m_pParam;
        m_pParam = NULL;
    }
    if(m_pTimerCheckDevice){
        m_pTimerCheckDevice->stop();
        delete m_pTimerCheckDevice;
        m_pTimerCheckDevice = NULL;
    }
}

void UserManage::quitManage()
{
    m_bQuit = true;
}

bool UserManage::isInit()
{
    return m_bInit;
}

int UserManage::QF_init()
{
    _StFunParamAndRes param;
    param.type = ft_init;
    waitThreadRunFinish(param);
    if(0 == param.nResult) m_bInit = true;
    return param.nResult;
}

void UserManage::QF_unInit()
{
    _StFunParamAndRes param;
    param.type = ft_unInit;
    waitThreadRunFinish(param);
}

int UserManage::QF_checkOnline()
{
    _StFunParamAndRes param;
    param.type = ft_checkOnline;
    waitThreadRunFinish(param);
    return param.nResult;
}

int UserManage::QF_connectDev()
{
    _StFunParamAndRes param;
    param.type = ft_connectDev;
    waitThreadRunFinish(param);
    if(0 == param.nResult){
        m_stDeviceStatus.bUseint = true;
        m_mutex.lock();
        m_stDeviceStatus.nLastTime = QDateTime::currentDateTime().toTime_t();
        m_mutex.unlock();
    }
    return param.nResult;
}

int UserManage::QF_disconnectDev()
{
    _StFunParamAndRes param;
    param.type = ft_disconnectDev;
    waitThreadRunFinish(param);
    if(0 == param.nResult){
        m_stDeviceStatus.bUseint = false;
        m_mutex.lock();
        m_stDeviceStatus.nLastTime = QDateTime::currentDateTime().toTime_t();
        m_mutex.unlock();
    }
    return param.nResult;
}

int UserManage::QF_checkPwd()
{
    _StFunParamAndRes param;
    param.type = ft_checkPwd;
    waitThreadRunFinish(param);
    return param.nResult;
}

int UserManage::QF_veriPassword(WCHAR *pUsername, WCHAR *pPassword)
{
    _StFunParamAndRes param;
    param.type = ft_veriPassword;
    param.pUserName = pUsername;
    param.pUserPass = pPassword;
    waitThreadRunFinish(param);
    return param.nResult;
}

int UserManage::QF_setPassword(WCHAR *pUsername, WCHAR *pPassword)
{
    _StFunParamAndRes param;
    param.type = ft_setPassword;
    param.pUserName = pUsername;
    param.pUserPass = pPassword;
    waitThreadRunFinish(param);
    return param.nResult;
}

int UserManage::QF_getUserName(WCHAR *pUser)
{
    _StFunParamAndRes param;
    param.type = ft_getUserName;
    param.pUserPass = pUser;
    waitThreadRunFinish(param);
    return param.nResult;
}

std::wstring UserManage::QF_failMsg(int errorCode)
{
    _StFunParamAndRes param;
    param.type = ft_failMsg;
    param.nCode = errorCode;
    waitThreadRunFinish(param);
    return param.wstr;
}

int UserManage::QF_lockScreen()
{
    _StFunParamAndRes param;
    param.type = ft_lockScreen;
    waitThreadRunFinish(param);
    return param.nResult;
}

int UserManage::QF_logout()
{
    _StFunParamAndRes param;
    param.type = ft_logout;
    waitThreadRunFinish(param);
    return param.nResult;
}

int UserManage::QF_reboot()
{
    _StFunParamAndRes param;
    param.type = ft_reboot;
    waitThreadRunFinish(param);
    return param.nResult;
}

int UserManage::QF_halt()
{
    _StFunParamAndRes param;
    param.type = ft_halt;
    waitThreadRunFinish(param);
    return param.nResult;
}

int UserManage::QF_userGet(StUser *pUser)
{
    _StFunParamAndRes param;
    param.type = ft_userGet;
    param.pUser = pUser;
    waitThreadRunFinish(param);
    return param.nResult;
}

StUserEx *UserManage::QF_getUserInfoByUserName(wchar_t *username)
{
    _StFunParamAndRes param;
    param.type = ft_getUserInfo;
    param.pUserName = username;
    waitThreadRunFinish(param);
    return param.pUserEx;
}

void UserManage::QF_freeUserInfo(StUserEx *pUser)
{
    _StFunParamAndRes param;
    param.type = ft_freeUserInfo;
    param.pUserEx = pUser;
    waitThreadRunFinish(param);
    return;
}

int UserManage::QF_userAdd(StUser *pUser)
{
    _StFunParamAndRes param;
    param.type = ft_userAdd;
    param.pUser = pUser;
    waitThreadRunFinish(param);
    return param.nResult;
}

int UserManage::QF_userDel(StUser *pUser)
{
    _StFunParamAndRes param;
    param.type = ft_userDel;
    param.pUser = pUser;
    waitThreadRunFinish(param);
    return param.nResult;
}

int UserManage::QF_veinAdd(StVein *pVein)
{
    _StFunParamAndRes param;
    param.type = ft_veinAdd;
    param.pVein = pVein;
    waitThreadRunFinish(param);
#ifdef _InterfaceDebug_
    param.nResult = 0;
#endif
    return param.nResult;
}

int UserManage::QF_veinDel(StVein *pVein)
{
    _StFunParamAndRes param;
    param.type = ft_veinDel;
    param.pVein = pVein;
    waitThreadRunFinish(param);
#ifdef _InterfaceDebug_
    param.nResult = 0;
#endif
    return param.nResult;
}

int UserManage::QF_delVein(int veinId)
{
    _StFunParamAndRes param;
    param.type = ft_delVein;
    param.nCode = veinId;
    waitThreadRunFinish(param);
    return param.nResult;
}

int UserManage::QF_checkFinger(int timeout, bool bCheckFinger)
{
    _StFunParamAndRes param;
    param.type = ft_checkFinger;
    param.nTimeOut = timeout;
    param.bCheckFinger = bCheckFinger;
    waitThreadRunFinish(param);
    return param.nResult;
}

long UserManage::QF_getVeinChara(CHAR *pChara)
{
    _StFunParamAndRes param;
    param.type = ft_getVeinChara;
    param.pChara1 = pChara;
    waitThreadRunFinish(param);
#ifdef _InterfaceDebug_
    char cc[10] = {"abcd"};
    memcpy(pChara, param.pChara1, 4);
#endif
    return param.fResult;
}

long UserManage::QF_createVeinTemp(CHAR *pChara1, CHAR *pChara2, CHAR *pChara3, CHAR *pVeinTemp)
{
    _StFunParamAndRes param;
    param.type = ft_createVeinTemp;
    param.pChara1 = pChara1;
    param.pChara2 = pChara2;
    param.pChara3 = pChara3;
    param.pTemp = pVeinTemp;
    waitThreadRunFinish(param);
#ifdef _InterfaceDebug_
    char cc[100] = {"hebingwancheng"};
    memcpy(pVeinTemp, param.pChara1, sizeof("hebingwancheng"));
    param.fResult = 111;
#endif
    return param.fResult;
}

int UserManage::QF_veinCompare(CHAR *pChara1, CHAR *pChara2, int score)
{
    _StFunParamAndRes param;
    param.type = ft_veinCompare;
    param.pChara1 = pChara1;
    param.pChara2 = pChara2;
    param.nCode = score;
    waitThreadRunFinish(param);
    return param.nResult;
}

int UserManage::QF_veinVerifyUser(CHAR *pChara, CHAR *temp)
{
    _StFunParamAndRes param;
    param.type = ft_veinVerifyUser;
    param.pChara1 = pChara;
    param.pTemp = temp;
    waitThreadRunFinish(param);
    return param.nResult;
}

int UserManage::QF_saveVeinTemp(StVein stVein)
{
    _StFunParamAndRes param;
    param.type = ft_saveVeinTemp;
    param.pVein = &stVein;
    waitThreadRunFinish(param);
    return param.nResult;
}

void UserManage::setDeviceOfflineTimeOut(unsigned int nTimeOut)
{
    //直接返回，不需要在此操作设备监控
    return;
    if(0 == nTimeOut) nTimeOut = 1;
    nTimeOut = nTimeOut*60;
    if(m_stDeviceStatus.nMaxDeviceOfflineTime == nTimeOut) return;
    m_stDeviceStatus.nMaxDeviceOfflineTime = nTimeOut;
    if(m_pTimerCheckDevice->isActive()){
        m_pTimerCheckDevice->stop();
        m_mutex.lock();
        m_stDeviceStatus.nLastTime = 0;
        m_mutex.unlock();
    }
    m_pTimerCheckDevice->start(10*1000);
}

bool UserManage::isDeviceUseIng()
{
    return m_stDeviceStatus.bUseint;
}

unsigned int UserManage::getLastTimeUseDevice()
{
    return m_stDeviceStatus.nLastTime;
}

void UserManage::updateLastTime()
{
    unsigned int nTime = QDateTime::currentDateTime().toTime_t();
    m_mutex.lock();
    m_stDeviceStatus.nLastTime = nTime;
    m_mutex.unlock();
}

void UserManage::onTimeOutCheckDecie()
{
    //先检查设备是否在用状态
    if(m_stDeviceStatus.bUseint) return;

    int nOnline = QF_checkOnline();
    if(nOnline > 0) {
        //设备在线直接跳出
        return;
    }
    if(0 == m_stDeviceStatus.nLastTime){
        m_mutex.lock();
        m_stDeviceStatus.nLastTime = QDateTime::currentDateTime().toTime_t();
        m_mutex.unlock();
    }
    m_mutex.lock();
    unsigned int nLastTime = m_stDeviceStatus.nLastTime;
    m_mutex.unlock();
    unsigned int nCurrentTime = QDateTime::currentDateTime().toTime_t();
    unsigned int nMaxTime = m_stDeviceStatus.nMaxDeviceOfflineTime;
    unsigned int nGapTime = nCurrentTime-nLastTime;
    if(nGapTime>nMaxTime){
        m_mutex.lock();
        m_stDeviceStatus.nLastTime = nCurrentTime;
        m_mutex.unlock();
        emit timeOutDeviceOffline();
    }
}

void UserManage::waitThreadRunFinish(_StFunParamAndRes &param, unsigned int timeout)
{
    if (NULL == m_pUserThread){
        param.nResult = -404;
        return;
    }
    //线程正在运行，说明上一次请求处理还没有结束
    unsigned int nStartTime = QDateTime::currentDateTime().toTime_t();
    while (m_pUserThread->isRunning()) {
        unsigned int nCurrentTime = QDateTime::currentDateTime().toTime_t();
        if(param.nTimeOut >= 0){
            if (nCurrentTime - nStartTime > timeout) {
                //等待超时
                param.nResult = -1024;
                return;
            }
        }
        QApplication::processEvents();
    }

    memset(m_pParam, 0, sizeof(_StFunParamAndRes));
    memcpy(m_pParam, &param, sizeof(_StFunParamAndRes));
    if (false == m_pUserThread->setFunParam(m_pParam)) {
        param.nResult = -404;
        return;
    }
    while (false == m_pUserThread->isFinished()) {
        unsigned int nCurrentTime = QDateTime::currentDateTime().toTime_t();
        if (nCurrentTime - nStartTime > timeout) {
            param.nResult = -1024;
            return;
        }
        QApplication::processEvents();
    }
    memcpy(&param, m_pParam, sizeof(_StFunParamAndRes));
}


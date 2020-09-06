#include "contorllthread.h"
#include <QDateTime>
#include <QDebug>

ContorllThread::ContorllThread(QObject *parent) : QThread(parent)
{
    
}

ContorllThread::~ContorllThread()
{

}
bool ContorllThread::setFunParam(_StFunParamAndRes* param)
{
    if (isRunning()) return false;
    m_pFunc = param;
    start();
    return true;
}

void ContorllThread::run()
{
    if(NULL == m_pFunc) return;
#ifdef _USER_DRV_API_
    switch (m_pFunc->type) {
    case ft_init:  thread_init(); break;
    case ft_unInit:  thread_unInit(); break;
    case ft_connectDev:  thread_connectDev(); break;
    case ft_disconnectDev:  thread_disconnectDev(); break;
    case ft_checkPwd:  thread_checkPwd(); break;
    case ft_veriPassword:  thread_veriPassword(); break;
    case ft_getUserName:  thread_getUserName(); break;
    case ft_setPassword: thread_setPassword(); break;
    case ft_failMsg:  thread_failMsg(); break;
    case ft_lockScreen:  thread_lockScreen(); break;
    case ft_logout:  thread_logout(); break;
    case ft_reboot:  thread_reboot(); break;
    case ft_halt:  thread_halt(); break;
    case ft_userGet:  thread_userGet(); break;
    case ft_getUserInfo: thread_getUserInfoByUserName(); break;
    case ft_freeUserInfo: thread_freeUserInfo(); break;
    case ft_userAdd:  thread_userAdd(); break;
    case ft_userDel:  thread_userDel(); break;
    case ft_veinAdd:  thread_veinAdd(); break;
    case ft_veinDel:  thread_veinDel(); break;
    case ft_delVein:  thread_delVein(); break;
    case ft_checkFinger:  thread_checkFinger(); break;
    case ft_getVeinChara:  thread_getVeinChara(); break;
    case ft_createVeinTemp:  thread_createVeinTemp(); break;
    case ft_veinCompare:  thread_veinCompare(); break;
    case ft_veinVerifyUser:  thread_veinVerifyUser(); break;
    case ft_saveVeinTemp:  thread_saveVeemp(); break;
    case ft_checkOnline: thread_checkOnline(); break;
    case ft_soundCtl: thread_soundCtl(); break;
    case ft_ledCtl: thread_ledCtl(); break;
    case ft_get11Status: thread_get11Status(); break;
        case ft_set11Status: thread_set11Status(); break;
        case ft_get1NStatus: thread_get1NStatus(); break;
        case ft_set1NStatus: thread_set1NStatus(); break;
    default:
        break;
    }
#else

#endif
}

#ifdef _USER_DRV_API_
void ContorllThread::thread_init()
{
    m_pFunc->nResult = init();
}

void ContorllThread::thread_unInit()
{
    unInit();
}

void ContorllThread::thread_connectDev()
{
    m_pFunc->nResult = connectDev();
#ifdef _InterfaceDebug_
    m_pFunc->nResult = 0;
#endif
}

void ContorllThread::thread_disconnectDev()
{
    m_pFunc->nResult = disconnectDev();
#ifdef _InterfaceDebug_
    m_pFunc->nResult = 0;
#endif
}

void ContorllThread::thread_checkPwd()
{
    m_pFunc->nResult = checkPwd();
}

void ContorllThread::thread_veriPassword()
{
    if(NULL == m_pFunc->pUserPass || NULL == m_pFunc->pUserName) return;
    m_pFunc->nResult = veriPassword(m_pFunc->pUserName, m_pFunc->pUserPass);
}

void ContorllThread::thread_getUserName()
{
    if(NULL == m_pFunc->pUserPass) return;

    m_pFunc->nResult = getUserName(m_pFunc->pUserPass);
}

void ContorllThread::thread_failMsg()
{
    m_pFunc->wstr = failMsg(m_pFunc->nCode);
}

void ContorllThread::thread_lockScreen()
{
    m_pFunc->nResult = lockScreen();
}

void ContorllThread::thread_logout()
{
    m_pFunc->nResult = logout();
}

void ContorllThread::thread_reboot()
{
    m_pFunc->nResult = reboot();
}

void ContorllThread::thread_halt()
{
    m_pFunc->nResult = halt();
}

void ContorllThread::thread_userGet()
{
    if(NULL == m_pFunc->pUser) return;

    //StUser user;
    //wcscpy(user.user_name, L"Administrator");
    //wcscpy(user.passwd, L"123456");
    int n = userGet(m_pFunc->pUser);
    qDebug()<<"---- user id"<<m_pFunc->pUser->user_id;
    m_pFunc->nResult = n;
}

void ContorllThread::thread_getUserInfoByUserName()
{
    if(NULL == m_pFunc->pUserName) return;
    m_pFunc->pUserEx = getUserInfoByUserName(m_pFunc->pUserName);
}

void ContorllThread::thread_freeUserInfo()
{
    if(NULL == m_pFunc->pUserEx) return;
    freeUserInfo(m_pFunc->pUserEx);
}

void ContorllThread::thread_setPassword()
{
#ifdef _InterfaceDebug_
    m_pFunc->nResult = -1;
#endif
    if(NULL == m_pFunc->pUserName || NULL == m_pFunc->pUserPass) return;
    m_pFunc->nResult = setPassword(m_pFunc->pUserName, m_pFunc->pUserPass);
}

void ContorllThread::thread_userAdd()
{
    if(NULL == m_pFunc->pUser) return;
    StUser user;
    //user.user_name = L"admin";
    //user.passwd = L"111";
    m_pFunc->nResult = userAdd(m_pFunc->pUser);
}

void ContorllThread::thread_userDel()
{
    if(NULL == m_pFunc->pUser) return;
    m_pFunc->nResult = userDel(m_pFunc->pUser);
}

void ContorllThread::thread_veinAdd()
{
    if(NULL == m_pFunc->pVein) return;
    qDebug()<<"---- veinAdd "<<m_pFunc->pVein->user_id<<m_pFunc->pVein->fg_id<<m_pFunc->pVein->temp;
    m_pFunc->nResult = veinAdd(m_pFunc->pVein);
}

void ContorllThread::thread_veinDel()
{
    if(NULL == m_pFunc->pVein) return;
    m_pFunc->nResult = veinDel(m_pFunc->pVein);
}

void ContorllThread::thread_delVein()
{
    //TODO
    //接口未实现
}

void ContorllThread::thread_checkFinger()
{
    //10秒内频繁调用
    int nr = -1;
    unsigned int nStartTime = QDateTime::currentDateTime().toTime_t();
    bool bRes = true;
    while(bRes){
        if(m_pFunc->nTimeOut > 0){
            if((QDateTime::currentDateTime().toTime_t()-nStartTime)>m_pFunc->nTimeOut){
                m_pFunc->nResult - 404;
                return;
            }
        }
        nr = checkFinger();
        if(m_pFunc->bCheckFinger){
            if(nr > 0){
                bRes = false;
            }
        } else {
            if(nr <= 0){
                bRes = false;
            }
        }
        msleep(100);
    }
    m_pFunc->nResult = nr;

#ifdef _InterfaceDebug_
    sleep(2);
    m_pFunc->nResult = 0;
#endif
}

void ContorllThread::thread_getVeinChara()
{
    if(NULL == m_pFunc->pChara1) return;
    m_pFunc->fResult = getVeinChara(m_pFunc->pChara1);
    qDebug()<<"---- getVeinChara:"<<m_pFunc->fResult;
#ifdef _InterfaceDebug_
    sleep(2);
    m_pFunc->pChara1 = {"aaa"};;
    m_pFunc->fResult = 111;
#endif
}

void ContorllThread::thread_createVeinTemp()
{
    if(NULL == m_pFunc->pChara1 || NULL == m_pFunc->pChara2
            || NULL == m_pFunc->pChara3 || NULL == m_pFunc->pTemp) return;
    m_pFunc->fResult = createVeinTemp(m_pFunc->pChara1, m_pFunc->pChara2, m_pFunc->pChara3, m_pFunc->pTemp);
}

void ContorllThread::thread_veinCompare()
{
    if(NULL == m_pFunc->pChara1 || NULL == m_pFunc->pChara2 ) return;
    m_pFunc->nResult = veinCompare(m_pFunc->pChara1, m_pFunc->pChara2, m_pFunc->nCode);
#ifdef _InterfaceDebug_
    sleep(2);
    m_pFunc->nResult = 0;
#endif
}

void ContorllThread::thread_veinVerifyUser()
{
    if(NULL == m_pFunc->pChara1 || NULL == m_pFunc->pTemp ) return;
    std::string strPchara = m_pFunc->pChara1;
    std::string strtemp = m_pFunc->pTemp;
    m_pFunc->nResult = veinVerifyUser(m_pFunc->pChara1, m_pFunc->pTemp);

    qDebug()<<strPchara.c_str();
    qDebug()<<strtemp.c_str();
    qDebug()<<"---- 匹配结果"<<m_pFunc->nResult;
}

void ContorllThread::thread_saveVeemp()
{
    //TODO
    //接口未实现
}

void ContorllThread::thread_checkOnline()
{
    m_pFunc->nResult = checkOnline();
}

void ContorllThread::thread_soundCtl()
{
    m_pFunc->nResult = soundCtl(m_pFunc->nCode);
}

void ContorllThread::thread_ledCtl()
{

}

void ContorllThread::thread_get11Status()
{
    m_pFunc->nResult = get11Status();
}

void ContorllThread::thread_set11Status()
{
    m_pFunc->nResult = set11Status(m_pFunc->nCode);
}

void ContorllThread::thread_get1NStatus()
{
    m_pFunc->nResult = get1NStatus();
}

void ContorllThread::thread_set1NStatus()
{
    m_pFunc->nResult = set1NStatus(m_pFunc->nCode);
}
#endif

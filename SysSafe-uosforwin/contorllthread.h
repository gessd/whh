#ifndef _CONTORLLTHREAD_H_
#define _CONTORLLTHREAD_H_
#include <QtCore/QThread>

//测试接口使用，正式版本去掉此定义
//#define _InterfaceDebug_

//方便调试接口库，正式版本需要此定义
#ifdef _USER_DRV_API_
#include "dfcx_common.h"
#else
#include <string>
#include <list>
struct StVein {
    int vein_id;
    int user_id;
    int fg_id;
    int null_id;
    std::string temp;
};

struct StUser {
    int user_id;
    std::wstring user_name;
    std::wstring passwd;
    std::list<StVein> veins;
};

struct StVeinEx {
    int vein_id;
    int user_id;
    int fg_id;
    int null_id;
    char temp[20*1024];
    StVeinEx *next;
};

/**
*  用户信息
*/
struct StUserEx {
    //主键
    int user_id;
    //用户名称
    wchar_t user_name[255];
    //密码
    wchar_t passwd[255];
    //指静脉信息
    StVeinEx *vein;
};

#endif

#ifndef __WCHAR_DEFINED
  typedef wchar_t WCHAR;
#endif
#ifndef CHAR
  typedef char CHAR;
#endif

enum _EnFunctionType
{
    ft_none =0,
    ft_init,
    ft_unInit,
    ft_connectDev,
    ft_disconnectDev,
    ft_checkPwd,
    ft_veriPassword,
    ft_setPassword,
    ft_getUserName,
    ft_failMsg,
    ft_lockScreen,
    ft_logout,
    ft_reboot,
    ft_halt,
    ft_userGet,
    ft_getUserInfo,
    ft_freeUserInfo,
    ft_userAdd,
    ft_userDel,
    ft_veinAdd,
    ft_veinDel,
    ft_delVein,
    ft_checkFinger,
    ft_getVeinChara,
    ft_createVeinTemp,
    ft_veinCompare,
    ft_veinVerifyUser,
    ft_saveVeinTemp,
    ft_checkOnline,
    ft_soundCtl,
    ft_ledCtl,
    ft_get11Status,
    ft_set11Status,
    ft_get1NStatus,
    ft_set1NStatus
};
struct _StFunParamAndRes
{
    QString qstrFuncName;
    _EnFunctionType type;
    int nTimeOut;
    bool bCheckFinger;

    //result
    int nResult;
    long lResult;
    std::wstring wstr;
    int fg_id;
    //param
    int nCode;
    char* pUserName;
    char* pUserPass;
    StVeinEx* pVeinEx;
    int Led[6];
    char* pChara1;
    char* pChara2;
    char* pChara3;
    char* pTemp;

    //result or param
    StUserEx* pUserEx;

    _StFunParamAndRes(){
        qstrFuncName.clear();
        type = ft_none;
        nTimeOut = 0;
        bCheckFinger = false;

        nResult = -1;
        lResult = -1;
        wstr = L"";
        fg_id = -1;

        nCode = 0;
        pUserName = pUserPass = NULL;
        pVeinEx = NULL;
        memset(Led, 0, sizeof(Led));

        pChara1 = pChara2 = pChara3 = pTemp =NULL;
        pUserEx = NULL;
    }
};

class ContorllThread : public QThread
{
    Q_OBJECT
public:
    ContorllThread(QObject *parent = NULL);
    ~ContorllThread();
	bool setFunParam(_StFunParamAndRes* param);

protected:
    void run();
#ifdef _USER_DRV_API_
private:
    void thread_init();                 //可用
    void thread_unInit();               //可用
    void thread_connectDev();           //可用
    void thread_disconnectDev();        //可用
    void thread_checkPwd();                         //废弃
    void thread_veriPassword();                     //废弃
    void thread_getUserName();                      //废弃
    void thread_failMsg();                          //废弃
    void thread_lockScreen();                       //废弃
    void thread_logout();                           //废弃
    void thread_reboot();                           //废弃
    void thread_halt();                             //废弃
    void thread_userGet();              //可用
    void thread_getUserInfoByUserName();     //未完成，参数fg_id
    void thread_freeUserInfo();         //可用
    void thread_setPassword();                      //废弃
    void thread_userAdd();              //可用
    void thread_userDel();              //可用
    void thread_veinAdd();              //可用
    void thread_veinDel();              //可用
    void thread_checkFinger();          //可用
    void thread_getVeinChara();         //可用
    void thread_createVeinTemp();       //可用
    void thread_veinCompare();          //可用
    void thread_veinVerifyUser();       //可用
    void thread_saveVeemp();                        //废弃
    void thread_checkOnline();                      //废弃
    void thread_soundCtl();             //可用
    void thread_ledCtl();               //可用
    void thread_get11Status();                      //废弃
    void thread_set11Status();                      //废弃
    void thread_get1NStatus();                      //废弃
    void thread_set1NStatus();                      //废弃
#endif
private:
    _StFunParamAndRes* m_pFunc;
};

#endif // CONTORLLTHREAD_H

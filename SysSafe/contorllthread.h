#ifndef _CONTORLLTHREAD_H_
#define _CONTORLLTHREAD_H_
#include <QtCore/QThread>

//测试接口使用，正式版本去掉此定义
#define _InterfaceDebug_

//方便调试接口库，正式版本需要此定义
#ifdef _USER_DRV_API_
#include "dfcx_common.h"
#include "dfcx_drv.h"
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
    ft_saveVeinTemp
};
struct _StFunParamAndRes
{
    QString qstrFuncName;
    _EnFunctionType type;
    int nTimeOut;
    bool bCheckFinger;
    //result
    int nResult;
    float fResult;
    std::wstring wstr;
    //param
    int nCode;
    WCHAR* pUserName;
    WCHAR* pUserPass;
    StUser* pUser;
    StVein* pVein;

    CHAR* pChara1;
    CHAR* pChara2;
    CHAR* pChara3;
    CHAR* pTemp;
    StUserEx* pUserEx;
    _StFunParamAndRes(){
        qstrFuncName.clear();
        type = ft_none;
        nTimeOut = 0;
        bCheckFinger = false;

        nResult = -1;
        fResult = -1;
        wstr = L"";

        nCode = 0;
        pUserName = pUserPass = NULL;
        pUser = NULL;
        pVein = NULL;

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
    void thread_init();
    void thread_unInit();
    void thread_connectDev();
    void thread_disconnectDev();
    void thread_checkPwd();
    void thread_veriPassword();
    void thread_getUserName();
    void thread_failMsg();
    void thread_lockScreen();
    void thread_logout();
    void thread_reboot();
    void thread_halt();
    void thread_userGet();
    void thread_getUserInfoByUserName();
    void thread_freeUserInfo();
    void thread_setPassword();
    void thread_userAdd();
    void thread_userDel();
    void thread_veinAdd();
    void thread_veinDel();
    void thread_delVein();
    void thread_checkFinger();
    void thread_getVeinChara();
    void thread_createVeinTemp();
    void thread_veinCompare();
    void thread_veinVerifyUser();
    void thread_saveVeemp();
#endif
private:
    _StFunParamAndRes* m_pFunc;
};

#endif // CONTORLLTHREAD_H

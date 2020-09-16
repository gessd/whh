#ifndef USERMANAGE_H
#define USERMANAGE_H

#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QUuid>
#include <QtCore/QTimer>
#include <QtCore/QMutex>
#include <QtWidgets/QToolButton>
#include "contorllthread.h"

enum VOICE_e
{
    VOICE_REG_OK = 0,//登记成功
    VOICE_USER_FULL = 1,//记录已满
    VOICE_REG_FAIL = 2,//登记失败
    VOICE_OVER_REG = 3, //登记重复
    VOICE_ADMIN_AUTH_OK=8, //管理员验证成功
    VOICE_ADMIN_AUTH_FAIL=9, //管理员验证失败
    VOICE_ERROR_CARD=10, //卡号错误
    VOICE_PWD_ERROR=13, //密码错误
    VOICE_PWD_REG_OK = 14,//密码登记成功
    VOICE_PWD_REG_FAIL = 15,//密码登记失败
    VOICE_ENROLL_ADMIN = 18,//请登记管理员
    VOICE_PINPUT_PWD = 19,//请输入密码
    VOICE_PINPUT_ID = 20,//请输入用户ID
    VOICE_ADMIN_AUTH = 21,//请验证管理员
    VOICE_INPUT_FINGER_AGAIN = 23,//请再放一次
    VOICE_RETRY = 24,//请再试一次
    VOICE_INPUT_AGAIN = 25,//请再输一次
    VOICE_RIGHT_INPUT = 26, //请正确放置手指
    VOICE_INPUT = 27,//请自然轻放手指
    VOICE_DEL_OK = 28,//删除成功
    VOICE_DEL_FAIL = 29,//删除失败
    VOICE_THANKS = 30, //谢谢
    VOICE_DELING = 31,//正在删除
    VOICE_IDENT_FAIL = 32,//验证失败
    VOICE_IDENT_OK = 33, //验证成功
    VOICE_UNLOCK = 34, //已开锁
    VOICE_BEEP1 = 35,
    VOICE_KEY = 36,
    VOICE_BEEP3 = 37,
    VOICE_BEEP4 = 38,
    VOICE_ALARM = 39, //39 报警音
    VOICE_OK_KEY_CONFIRM = 40, //按OK键确认
    VOICE_POWER_LOW = 41, //电量不足请更换电池
    VOICE_ADMIN = 42,//管理员
    VOICE_PWD = 43,//密码
    VOICE_CARDNO = 44,//刷卡
    VOICE_INPUT_CARD = 45, //请刷卡
    VOICE_PLS_INPUT_ADMIN_PWD = 47, //请输入管理员密码
    VOICE_ADMIN_FULL = 49, //管理员已满
    VOICE_OK_KEY_DEL_CONFIRM = 50, //请按OK键确认删除
    VOICE_END
};

struct StDeviceStatus
{
    bool bUseint;           //使用中
    unsigned int nLastTime; //上次连接设备时间 UTC时间
    unsigned int nMaxDeviceOfflineTime; //设备最长离线时间
    StDeviceStatus() {
        bUseint = false;
        nLastTime = 0;
        nMaxDeviceOfflineTime = 0;
    }
};

class UserManage : public QObject
{
    Q_OBJECT
public:
    explicit UserManage(QObject *parent = NULL);
    ~UserManage();
    void quitManage();
    bool isInit();
    /**
        全局初始化
    */
    int QF_init();


    /**
    *	全局释放
    */
    void QF_unInit();

    /**
    *	检测设备在线状态
    *	return	@ >0 在线
    *			@ <0 不在线
    */
    int QF_checkOnline();
    /**
    *	连接设备
    */
    int QF_connectDev();


    /**
    *	断开连接
    */
    int QF_disconnectDev();


    /**
    *	检查是否为空密码
    *	空密码为0, 不是空密码返回非0
    */
    int QF_checkPwd(void);

    /**
    *	验证密码
    */
    int QF_veriPassword(WCHAR *pUsername, WCHAR *pPassword);
    int QF_setPassword(WCHAR *pUsername, WCHAR *pPassword);

    /**
    *	获取当前用户
    */
    int QF_getUserName(WCHAR *pUser);

    /**
    *	获取错误信息
    */
    std::wstring QF_failMsg(int errorCode);

    /**
    *	锁屏
    */
    int QF_lockScreen();


    /**
    *	注销
    */
    int QF_logout();


    /**
    *	重启
    */
    int QF_reboot();


    /**
    *	关机
    */
    int QF_halt();


    /**
    *	获取用户信息
    */
    int QF_userGet(StUser *pUser);

    StUserEx *QF_getUserInfoByUserName(wchar_t *username);

    void QF_freeUserInfo(StUserEx *pUser);

    /**
    *	添加、修改用户信息
    */
    int QF_userAdd(StUser *pUser);

    /**
    *	删除用户信息
    */
    int QF_userDel(StUser *pUser);


    /**
    *	保存指静脉信息
    */
    int QF_veinAdd(StVein *pVein);

    /**
    *	删除指静脉信息
    */
    int QF_veinDel(StVein *pVein);

    /**
     * 删除用户指静脉信息
     */
    int QF_delVein(int veinId);

    /**
     *	检测手指	首先需要连接设备，
     */
    int QF_checkFinger(int timeout, bool bCheckFinger);

    /**
     *	获取特征	首先需要连接设备
     */
    long QF_getVeinChara(CHAR *pChara);

    /**
     *	融合模板
     */
    long QF_createVeinTemp(CHAR *pChara1, CHAR *pChara2, CHAR *pChara3, CHAR *pVeinTemp);

    /**
     *	特征对比
     */
    int QF_veinCompare(CHAR *pChara1, CHAR *pChara2, int score);

    /**
     *	1:1验证
     */
    int QF_veinVerifyUser(CHAR *pChara, CHAR *temp);

    /**
     *	保存特征
     */
    int QF_saveVeinTemp(StVein stVein);


    int QF_soundCtl(int Sound);

    int QF_ledCtl(int Led[6]);

    /**
       * @brief 设置设备超时时间
       * @param nTimeOut 分钟
       */
    void setDeviceOfflineTimeOut(unsigned int nTimeOut);
    /**
       * @brief isDeviceUseIng
       * @return 设备是否在用状态
       */
    bool isDeviceUseIng();
    /**
       * @brief getLastTimeUseDevice
       * @return 设备上次使用时间
       */
    unsigned int getLastTimeUseDevice();

    /**
       * @brief 更新上次响应时间
       */
    void updateLastTime();

    /**
    *	获取1:1验证状态
    *	返回0	未启用
    *	返回1	已启用
    */
    int QF_get11Status();

    /**
    *	设置1:1验证状态
    *	返回0	设置成功
    *	返回<0	设置失败
    */
    int QF_set11Status(int status);

    /**
    *	获取1:N验证状态
    *	返回0	未启用
    *	返回1	已启用
    */
    int QF_get1NStatus();

    /**
    *	设置1:N验证状态
    *	返回0	设置成功
    *	返回<0	设置失败
    */
    int QF_set1NStatus(int status);
private slots:
    void onTimeOutCheckDecie();
signals:
    void timeOutDeviceOffline();
private:
    void waitThreadRunFinish(_StFunParamAndRes& param, unsigned int timeout = 10);
private:
    bool m_bInit;
    bool m_bQuit;
    bool m_bDeviceConnect;
    _StFunParamAndRes* m_pParam;
    ContorllThread* m_pUserThread;
    QTimer* m_pTimerCheckDevice;
    //设备状态
    StDeviceStatus m_stDeviceStatus;
    QMutex m_mutex;
};

#endif // USERMANAGE_H

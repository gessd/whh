#ifndef USERMANAGE_H
#define USERMANAGE_H

#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QUuid>
#include <QtCore/QTimer>
#include <QtWidgets/QToolButton>
#include "contorllthread.h"

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

      /**
       * @brief 设置设备超时时间
       * @param nTimeOut 秒
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
};

#endif // USERMANAGE_H

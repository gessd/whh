// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 DFCX_DRV_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// DFCX_DRV_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef DFCX_DRV_EXPORTS
#define DFCX_DRV_API __declspec(dllexport)
#else
#define DFCX_DRV_API __declspec(dllimport)
#endif

#include <Windows.h>
#include "dfcx_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
*	全局初始化
*	return	@ = 0 成功
*			@ < 0 失败
*/
DFCX_DRV_API int init();

/**
*	全局释放
*/
DFCX_DRV_API void unInit();

/**
*	检测设备在线状态
*	return	@ >0 在线
*			@ <0 不在线
*/
DFCX_DRV_API int checkOnline();

/**
*	连接设备
*/
DFCX_DRV_API int connectDev();

/**
*	断开连接
*/
DFCX_DRV_API int disconnectDev();


/**
*	检查是否为空密码
*	空密码为0, 不是空密码返回非0
*/
DFCX_DRV_API int checkPwd(void);

/**
*	验证密码
*/
DFCX_DRV_API int veriPassword(WCHAR *pUsername, WCHAR *pPassword);

/**
*	设置密码
*/
DFCX_DRV_API int setPassword(WCHAR *pUsername, WCHAR *pPassword);

/**
*	获取当前用户
*/
DFCX_DRV_API int getUserName(WCHAR *pUser);

/**
*	获取错误信息
*/
DFCX_DRV_API std::wstring failMsg(int errorCode);


/**
*	锁屏
*/
DFCX_DRV_API int lockScreen();


/**
*	注销
*/
DFCX_DRV_API int logout();


/**
*	重启
*/
DFCX_DRV_API int reboot();


/**
*	关机
*/
DFCX_DRV_API int halt();


/**
*	获取用户信息
*/
DFCX_DRV_API int userGet(StUser *pUser);

/** 获取用户信息
*	包含指静脉链表
*/
DFCX_DRV_API StUserEx *getUserInfoByUserName(wchar_t *username);


DFCX_DRV_API void freeUserInfo(StUserEx *pUser);

/**
*	添加、修改用户信息
*/
DFCX_DRV_API int userAdd(StUser *pUser);

/**
*	删除用户信息
*/
DFCX_DRV_API int userDel(StUser *pUser);


/**
*	保存指静脉信息
*/
DFCX_DRV_API int veinAdd(StVein *pVein);

/**
*	删除指静脉信息
*/
DFCX_DRV_API int veinDel(StVein *pVein);



/**
* 删除用户指静脉信息
*/
DFCX_DRV_API int delVein(int veinId);

/**
*	检测手指	首先需要连接设备，
*/
DFCX_DRV_API int checkFinger();

/**
*	获取特征	首先需要连接设备
*/
DFCX_DRV_API long getVeinChara(CHAR *pChara);

/**
*	融合模板
*/
DFCX_DRV_API long createVeinTemp(CHAR *pChara1, CHAR *pChara2, CHAR *pChara3, CHAR *pVeinTemp);

/**
*	特征对比
*/
DFCX_DRV_API int veinCompare(CHAR *pChara1, CHAR *pChara2, int score);

/**
*	1:1验证
*/
DFCX_DRV_API int veinVerifyUser(CHAR *pChara, CHAR *temp);

/**
*	1:N 验证
*/
DFCX_DRV_API StUserEx *veinSearchUser(CHAR *pChara);

/**
*	保存特征
*/
DFCX_DRV_API int saveVeinTemp(StVein stVein);

DFCX_DRV_API int soundCtl( int Sound);

//Led[6]对应6个灯，0=亮灯，1=灭灯
DFCX_DRV_API int ledCtl(int Led[6]);

#ifdef __cplusplus
};
#endif
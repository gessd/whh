// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� DFCX_DRV_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// DFCX_DRV_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
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
*	ȫ�ֳ�ʼ��
*	return	@ = 0 �ɹ�
*			@ < 0 ʧ��
*/
DFCX_DRV_API int init();

/**
*	ȫ���ͷ�
*/
DFCX_DRV_API void unInit();

/**
*	����豸����״̬
*	return	@ >0 ����
*			@ <0 ������
*/
DFCX_DRV_API int checkOnline();

/**
*	�����豸
*/
DFCX_DRV_API int connectDev();

/**
*	�Ͽ�����
*/
DFCX_DRV_API int disconnectDev();


/**
*	����Ƿ�Ϊ������
*	������Ϊ0, ���ǿ����뷵�ط�0
*/
DFCX_DRV_API int checkPwd(void);

/**
*	��֤����
*/
DFCX_DRV_API int veriPassword(WCHAR *pUsername, WCHAR *pPassword);

/**
*	��������
*/
DFCX_DRV_API int setPassword(WCHAR *pUsername, WCHAR *pPassword);

/**
*	��ȡ��ǰ�û�
*/
DFCX_DRV_API int getUserName(WCHAR *pUser);

/**
*	��ȡ������Ϣ
*/
DFCX_DRV_API std::wstring failMsg(int errorCode);


/**
*	����
*/
DFCX_DRV_API int lockScreen();


/**
*	ע��
*/
DFCX_DRV_API int logout();


/**
*	����
*/
DFCX_DRV_API int reboot();


/**
*	�ػ�
*/
DFCX_DRV_API int halt();


/**
*	��ȡ�û���Ϣ
*/
DFCX_DRV_API int userGet(StUser *pUser);

/** ��ȡ�û���Ϣ
*	����ָ��������
*/
DFCX_DRV_API StUserEx *getUserInfoByUserName(wchar_t *username);


DFCX_DRV_API void freeUserInfo(StUserEx *pUser);

/**
*	��ӡ��޸��û���Ϣ
*/
DFCX_DRV_API int userAdd(StUser *pUser);

/**
*	ɾ���û���Ϣ
*/
DFCX_DRV_API int userDel(StUser *pUser);


/**
*	����ָ������Ϣ
*/
DFCX_DRV_API int veinAdd(StVein *pVein);

/**
*	ɾ��ָ������Ϣ
*/
DFCX_DRV_API int veinDel(StVein *pVein);



/**
* ɾ���û�ָ������Ϣ
*/
DFCX_DRV_API int delVein(int veinId);

/**
*	�����ָ	������Ҫ�����豸��
*/
DFCX_DRV_API int checkFinger();

/**
*	��ȡ����	������Ҫ�����豸
*/
DFCX_DRV_API long getVeinChara(CHAR *pChara);

/**
*	�ں�ģ��
*/
DFCX_DRV_API long createVeinTemp(CHAR *pChara1, CHAR *pChara2, CHAR *pChara3, CHAR *pVeinTemp);

/**
*	�����Ա�
*/
DFCX_DRV_API int veinCompare(CHAR *pChara1, CHAR *pChara2, int score);

/**
*	1:1��֤
*/
DFCX_DRV_API int veinVerifyUser(CHAR *pChara, CHAR *temp);

/**
*	1:N ��֤
*/
DFCX_DRV_API StUserEx *veinSearchUser(CHAR *pChara);

/**
*	��������
*/
DFCX_DRV_API int saveVeinTemp(StVein stVein);

DFCX_DRV_API int soundCtl( int Sound);

//Led[6]��Ӧ6���ƣ�0=���ƣ�1=���
DFCX_DRV_API int ledCtl(int Led[6]);

#ifdef __cplusplus
};
#endif
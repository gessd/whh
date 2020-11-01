#ifndef Q_OS_WIN
#include "dfcx_drv.h"
#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <syslog.h>
#include "userservice.h"

typedef char *LPCTSTR;

#define	XG_ERR_SUCCESS                0x00 
#define	XG_ERR_FAIL                   0x01 
#define XG_ERR_COM                    0x02 
#define XG_ERR_DATA                   0x03 
#define XG_ERR_INVALID_PWD            0x04 
#define XG_ERR_INVALID_PARAM          0x05 
#define XG_ERR_INVALID_ID             0x06 
#define XG_ERR_EMPTY_ID               0x07 
#define XG_ERR_NOT_ENOUGH             0x08 
#define XG_ERR_NO_SAME_FINGER         0x09 
#define XG_ERR_DUPLICATION_ID         0x0A 
#define XG_ERR_TIME_OUT               0x0B 
#define XG_ERR_VERIFY                 0x0C 
#define XG_ERR_NO_NULL_ID             0x0D 
#define XG_ERR_BREAK_OFF              0x0E 
#define XG_ERR_NO_CONNECT             0x0F 
#define XG_ERR_NO_SUPPORT             0x10 
#define XG_ERR_NO_VEIN                0x11 
#define XG_ERR_MEMORY                 0x12 
#define XG_ERR_NO_DEV                 0x13 
#define XG_ERR_ADDRESS                0x14 
#define XG_ERR_NO_FILE                0x15 
#define XG_ERR_VER                    0x16 
#define XG_ERR_PACKET_PREFIX          0x30 
#define XG_ERR_PACKET_CHECK           0x31 
#define XG_ERR_PACKET_LACK            0x32 

typedef struct _API_FV
{  
	long (*FV_SetDebug) (long on);
	long (*FV_ConnectDev) (LPCTSTR sDev, LPCTSTR sPasswrod); 
	long (*FV_CloseDev) (long lDevHandle);
	long (*FV_SendCmdPacket) (long lDevHandle, long lCmd, LPCTSTR sData);
	long (*FV_RecvCmdPacket) (long lDevHandle, LPCTSTR sData, long lTimeout);
	long (*FV_RecvUartData) (long lDevHandle, LPCTSTR sData, long lSize, long lTimeout);
	long (*FV_GetDevParam) (long lDevHandle, LPCTSTR sParam);
	long (*FV_SetDevParam) (long lDevHandle, LPCTSTR sParam);
	long (*FV_PlayDevSound) (long lDevHandle, long lSound);
	long (*FV_CheckFinger) (long lDevHandle);
	long (*FV_GetVeinChara) (long lDevHandle, LPCTSTR sChara, long lTimeout);
	long (*FV_ReadDevTemp) (long lDevHandle, long lUserId, LPCTSTR sTemp);
	long (*FV_WriteDevTemp) (long lDevHandle, long lUserId, LPCTSTR sTemp, LPCTSTR sUserInfo);
	long (*FV_DeleteDevTemp) (long lDevHandle, long lUserId);
	long (*FV_GetDevDebugInfo) (long lDevHandle, LPCTSTR sFileName);
	long (*FV_Upgrade) (long lDevHandle, LPCTSTR sFileName);
	long (*FV_GetImgFormData) (LPCTSTR sData, long lDataLen, LPCTSTR sImg);
	long (*FV_GetImgFormDev) (long lDevHandle, LPCTSTR sImg);
	long (*FV_GetCharaFromImg) (LPCTSTR sImg, LPCTSTR sChara);
	long (*FV_ReadDevLog) (long lDevHandle, LPCTSTR sStartTime, LPCTSTR sEndTime, LPCTSTR sLog);
	long (*FV_WriteDevUserInfo) (long lDevHandle, long lStartID, long lNum, LPCTSTR sUserInfo);
	long (*FV_ReadDevUserInfo) (long lDevHandle, long lStartID, long lNum, LPCTSTR sUserInfo);

	long (*FV_CreateVeinLib) (long lUserNum);
	long (*FV_DestroyVeinLib) (long lLibHandle);
	long (*FV_GetNullID)(long lLibHandle);
	long (*FV_ImportVeinTemp) (long lLibHandle, long lUserId, LPCTSTR sTemp, LPCTSTR sUserInfo);
	long (*FV_ExportVeinTemp) (long lLibHandle, long lUserId, LPCTSTR sTemp);
	long (*FV_CleanVeinTemp) (long lLibHandle, long lUserId);
	long (*FV_SetUserInfo) (long lLibHandle, long lUserId, LPCTSTR sUserInfo);
	long (*FV_GetUserInfo) (long lLibHandle, long lUserId, LPCTSTR sUserInfo);
	long (*FV_GetTempUserInfo) (LPCTSTR sTemp, LPCTSTR sUserInfo);
	long (*FV_SearchUser) (long lLibHandle, LPCTSTR sChara, long lTh, LPCTSTR sUserInfo);
	long (*FV_VerifyUser) (LPCTSTR sTemp, LPCTSTR sChara, long lScore, LPCTSTR sStudyTemp, LPCTSTR sUserInfo);
	long (*FV_CreateVeinTemp) (LPCTSTR sChara1, LPCTSTR sChara2, LPCTSTR sChara3, LPCTSTR sTemp, LPCTSTR sUserInfo);
	long (*FV_CharaMatch) (LPCTSTR sChara1, LPCTSTR sChara2, long Score);

	long (*FV_NetPackParse) (LPCTSTR bNetBuf, long lBufSize, LPCTSTR sCmd, LPCTSTR sSN, LPCTSTR sData);
	long (*FV_AsciiToHex) (LPCTSTR sAscii, LPCTSTR bHex);
	long (*FV_HexToAscii) (LPCTSTR bHex, long lByte, LPCTSTR sAscii);

	long (*FV_SocketServerInit) (long lPort, LPCTSTR sParam);
	long (*FV_SocketAccept) (long lSocket, LPCTSTR sInfo);
	long (*FV_SocketRecvPack) (long lSocket, LPCTSTR sCMD, LPCTSTR sSN, LPCTSTR sData);
	long (*FV_SocketSendPack) (long lSocket, long lCmd, LPCTSTR sData);
	long (*FV_SocketClose) (long lSocket);

} FVAPI_t, *pFVAPI_t;

static long fvLibHandle = 0;
static long fvDevHandle = 0;
static FVAPI_t m_FVAPI;

static void* GetFVAPI(void *pSoHandle, const char* sApiName)
{
	char* sError = NULL;
	void* pApi = NULL;

	if(pSoHandle == NULL || sApiName == NULL) return NULL;

	pApi = dlsym(pSoHandle, sApiName);
	sError = dlerror();
	if(sError != NULL) 
	{
		return NULL;
	}
	return pApi;
}

int init()
{
	char* sSoFile = "/usr/lib/libXGComApi.so";
	char* sError = NULL;
	void *pSoHandle = NULL;

	pSoHandle = dlopen(sSoFile, RTLD_LAZY);
	if(pSoHandle == NULL)
	{
		syslog(LOG_USER, "so file not exist");
		return -1;
	} else {
		memset(&m_FVAPI, 0, sizeof(FVAPI_t));
		m_FVAPI.FV_SetDebug = (long(*)(long))GetFVAPI(pSoHandle, "FV_SetDebug");
		m_FVAPI.FV_ConnectDev = (long(*)(LPCTSTR,LPCTSTR))GetFVAPI(pSoHandle, "FV_ConnectDev");
		m_FVAPI.FV_CloseDev = (long(*)(long))GetFVAPI(pSoHandle, "FV_CloseDev");
		m_FVAPI.FV_SendCmdPacket = (long(*)(long,long,LPCTSTR))GetFVAPI(pSoHandle, "FV_SendCmdPacket");
		m_FVAPI.FV_RecvCmdPacket = (long(*)(long,LPCTSTR,long))GetFVAPI(pSoHandle, "FV_RecvCmdPacket");
		m_FVAPI.FV_RecvUartData = (long(*)(long,LPCTSTR,long,long))GetFVAPI(pSoHandle, "FV_RecvUartData");
		m_FVAPI.FV_GetDevParam = (long(*)(long,LPCTSTR))GetFVAPI(pSoHandle, "FV_GetDevParam");
		m_FVAPI.FV_SetDevParam = (long(*)(long,LPCTSTR))GetFVAPI(pSoHandle, "FV_SetDevParam");
		m_FVAPI.FV_PlayDevSound = (long(*)(long,long))GetFVAPI(pSoHandle, "FV_PlayDevSound");
		m_FVAPI.FV_CheckFinger = (long(*)(long))GetFVAPI(pSoHandle, "FV_CheckFinger");
		m_FVAPI.FV_GetVeinChara = (long(*)(long,LPCTSTR,long))GetFVAPI(pSoHandle, "FV_GetVeinChara");
		m_FVAPI.FV_ReadDevTemp = (long(*)(long,long,LPCTSTR))GetFVAPI(pSoHandle, "FV_ReadDevTemp");
		m_FVAPI.FV_WriteDevTemp = (long(*)(long,long,LPCTSTR,LPCTSTR))GetFVAPI(pSoHandle, "FV_WriteDevTemp");
		m_FVAPI.FV_DeleteDevTemp = (long(*)(long,long))GetFVAPI(pSoHandle, "FV_DeleteDevTemp");
		m_FVAPI.FV_GetDevDebugInfo = (long(*)(long,LPCTSTR))GetFVAPI(pSoHandle, "FV_GetDevDebugInfo");
		m_FVAPI.FV_Upgrade = (long(*)(long,LPCTSTR))GetFVAPI(pSoHandle, "FV_Upgrade");
		m_FVAPI.FV_GetImgFormData = (long(*)(LPCTSTR,long,LPCTSTR))GetFVAPI(pSoHandle, "FV_GetImgFormData");
		m_FVAPI.FV_GetImgFormDev = (long(*)(long,LPCTSTR))GetFVAPI(pSoHandle, "FV_GetImgFormDev");
		m_FVAPI.FV_GetCharaFromImg = (long(*)(LPCTSTR,LPCTSTR))GetFVAPI(pSoHandle, "FV_GetCharaFromImg");
		m_FVAPI.FV_ReadDevLog = (long(*)(long,LPCTSTR,LPCTSTR,LPCTSTR))GetFVAPI(pSoHandle, "FV_ReadDevLog");
		m_FVAPI.FV_WriteDevUserInfo = (long(*)(long,long,long,LPCTSTR))GetFVAPI(pSoHandle, "FV_WriteDevUserInfo");
		m_FVAPI.FV_ReadDevUserInfo = (long(*)(long,long,long,LPCTSTR))GetFVAPI(pSoHandle, "FV_ReadDevUserInfo");

		m_FVAPI.FV_CreateVeinLib = (long(*)(long))GetFVAPI(pSoHandle, "FV_CreateVeinLib");
		m_FVAPI.FV_DestroyVeinLib = (long(*)(long))GetFVAPI(pSoHandle, "FV_DestroyVeinLib");
		m_FVAPI.FV_GetNullID = (long(*)(long))GetFVAPI(pSoHandle, "FV_GetNullID");
		m_FVAPI.FV_ImportVeinTemp = (long(*)(long,long,LPCTSTR,LPCTSTR))GetFVAPI(pSoHandle, "FV_ImportVeinTemp");
		m_FVAPI.FV_ExportVeinTemp = (long(*)(long,long,LPCTSTR))GetFVAPI(pSoHandle, "FV_ExportVeinTemp");
		m_FVAPI.FV_CleanVeinTemp = (long(*)(long,long))GetFVAPI(pSoHandle, "FV_CleanVeinTemp");
		m_FVAPI.FV_SetUserInfo = (long(*)(long,long,LPCTSTR))GetFVAPI(pSoHandle, "FV_SetUserInfo");
		m_FVAPI.FV_GetUserInfo = (long(*)(long,long,LPCTSTR))GetFVAPI(pSoHandle, "FV_GetUserInfo");
		m_FVAPI.FV_GetTempUserInfo = (long(*)(LPCTSTR,LPCTSTR))GetFVAPI(pSoHandle, "FV_GetTempUserInfo");
		m_FVAPI.FV_SearchUser = (long(*)(long,LPCTSTR,long,LPCTSTR))GetFVAPI(pSoHandle, "FV_SearchUser");
		m_FVAPI.FV_VerifyUser = (long(*)(LPCTSTR,LPCTSTR,long,LPCTSTR,LPCTSTR))GetFVAPI(pSoHandle, "FV_VerifyUser");
		m_FVAPI.FV_CreateVeinTemp = (long(*)(LPCTSTR,LPCTSTR,LPCTSTR,LPCTSTR,LPCTSTR))GetFVAPI(pSoHandle, "FV_CreateVeinTemp");
		m_FVAPI.FV_CharaMatch = (long(*)(LPCTSTR,LPCTSTR,long))GetFVAPI(pSoHandle, "FV_CharaMatch");

		m_FVAPI.FV_NetPackParse = (long(*)(LPCTSTR,long,LPCTSTR,LPCTSTR,LPCTSTR))GetFVAPI(pSoHandle, "FV_NetPackParse");
		m_FVAPI.FV_AsciiToHex = (long(*)(LPCTSTR,LPCTSTR))GetFVAPI(pSoHandle, "FV_AsciiToHex");
		m_FVAPI.FV_HexToAscii = (long(*)(LPCTSTR,long,LPCTSTR))GetFVAPI(pSoHandle, "FV_HexToAscii");

		m_FVAPI.FV_SocketServerInit = (long(*)(long,LPCTSTR))GetFVAPI(pSoHandle, "FV_SocketServerInit");
		m_FVAPI.FV_SocketAccept = (long(*)(long,LPCTSTR))GetFVAPI(pSoHandle, "FV_SocketAccept");
		m_FVAPI.FV_SocketRecvPack = (long(*)(long,LPCTSTR,LPCTSTR,LPCTSTR))GetFVAPI(pSoHandle, "FV_SocketRecvPack");
		m_FVAPI.FV_SocketSendPack = (long(*)(long,long,LPCTSTR))GetFVAPI(pSoHandle, "FV_SocketSendPack");
		m_FVAPI.FV_SocketClose = (long(*)(long))GetFVAPI(pSoHandle, "FV_SocketClose");
	}

	fvLibHandle = m_FVAPI.FV_CreateVeinLib(1000);

	if (fvLibHandle <= 0) {
		return -1;
	}
	m_FVAPI.FV_SetDebug(1);

	StUserEx *pUser = getUsers();
	StUserEx *pTmp = pUser;

	while (pTmp != NULL) {

		StVeinEx *pVein = pTmp->vein;
		
		while (pVein != NULL) {
			long nullId = m_FVAPI.FV_GetNullID(fvLibHandle);

			if (nullId <= 0) {
				return -1;
			}

			pVein->null_id = nullId;

			int ret = addVein(pVein);

			if (ret < 0) {
				return -1;
			}

			long result = m_FVAPI.FV_ImportVeinTemp(fvLibHandle, nullId, pVein->temp, NULL);

			pVein = pVein->next;
		}
		pTmp = pTmp->next;
	}
	free(pUser);
	return 0;
}


/**
*	全局释放
*/
 void unInit()
{
	if (fvLibHandle > 0) {
		m_FVAPI.FV_DestroyVeinLib(fvLibHandle);
	}
}

/**
*	连接设备
*/
 int connectDev()
{
	if (fvDevHandle > 0) {
		return 0;
	}

	fvDevHandle = m_FVAPI.FV_ConnectDev((char*)"VID=8455,PID=30008", (char*)"00000000");
	if (fvDevHandle <= 0) {
		syslog(LOG_USER, "connectDev return %d\n", fvDevHandle);
		return -1;
	}

	return 0;
}


/**
*	断开连接
*/
 int disconnectDev()
{
	if (fvDevHandle <= 0) {
		return -1;
	}
	m_FVAPI.FV_CloseDev(fvDevHandle);
	fvDevHandle = 0;

	return 0;
}


 long createVeinTemp(char *pChara1, char *pChara2, char *pChara3, char *pVeinTemp)
{
	return m_FVAPI.FV_CreateVeinTemp(pChara1, pChara2, pChara3, pVeinTemp, NULL);
}

/**
*	特征对比
*/
 int veinCompare(char *pChara1, char *pChara2, int score)
{
	long len = 0;
	char tmp[20*1024] = {0};

	len = m_FVAPI.FV_CharaMatch(pChara1, pChara2, score);

	if (len < 0) {
		return -1;
	}

	return 0;
}

/**
*	检测手指
*/
 int checkFinger()
{
	if (fvDevHandle <= 0) {
		return -1;
	}
	return m_FVAPI.FV_CheckFinger(fvDevHandle);
}

long getVeinChara(char *pChara)
{
	long lTimeout = 10*1000;
	return m_FVAPI.FV_GetVeinChara(fvDevHandle, pChara, lTimeout);
}

int veinVerifyUser(char *pChara, char *temp)
{
	char tmp[20*1024] = {0};
	long ret = m_FVAPI.FV_VerifyUser(temp, pChara, 70, tmp, NULL);

	if (ret > 0) {
		return 0;
	}
	return -1;
}


int soundCtl(int Sound)
{
	if (fvDevHandle <= 0) {
		return -1;
	}
	int ret;
	unsigned char bData[16] = { 0 };
	char sData[50] = { 0 };

	sprintf(sData, "20%02X", Sound);
	ret = m_FVAPI.FV_SendCmdPacket(fvDevHandle, 0x4B, sData);
	if(ret == XG_ERR_SUCCESS)
	{
		m_FVAPI.FV_RecvCmdPacket(fvDevHandle, sData, 1000);
	}
	return ret*-1;
}

int ledCtl(int Led[6])
{
	int ret;
	unsigned char bData[16] = { 0 };
	char sData[50] = { 0 };

	if (fvDevHandle <= 0) {
		return -1;
	}

	sprintf(sData, "21%02X%02X%02X%02X%02X%02X", Led[0], Led[1], Led[2], Led[3], Led[4], Led[5]);
	ret = m_FVAPI.FV_SendCmdPacket(fvDevHandle, 0x4B, sData);
	if(ret == XG_ERR_SUCCESS)
	{
		m_FVAPI.FV_RecvCmdPacket(fvDevHandle, sData, 1000);
	}
	return ret*-1;
}

#endif

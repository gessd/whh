// dfcx_drv_test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "dfcx_drv.h"


#include <Windows.h>
#include <lm.h>
#pragma comment(lib, "netapi32.lib")

static void testVeinAdd(StUser *pUser);
static void testVeinCheck(StUser *pUser);
static void testVeinDel(StUser *pUser);
static void testSetPassword();
static void testSearchUser();

void getAllUser()
{
	USER_INFO_2* l_pUserInfo=NULL;
	DWORD l_dwentriesread=0;
	DWORD l_dwtotalentries=0;
	NetUserEnum(NULL,2,FILTER_NORMAL_ACCOUNT,(LPBYTE*)&l_pUserInfo,MAX_PREFERRED_LENGTH,&l_dwentriesread,&l_dwtotalentries,NULL);
	for(int i=0; i<l_dwentriesread; i++) {
		if (!(UF_ACCOUNTDISABLE & (l_pUserInfo+i)->usri2_flags) && (UF_PASSWD_NOTREQD & (l_pUserInfo+i)->usri2_flags)) {
				//不是禁用用户，不是无密码用户
			printf("UserName:%ls %ls %d %d\n",(l_pUserInfo+i)->usri2_name, (l_pUserInfo+i)->usri2_password, (l_pUserInfo+i)->usri2_flags,
			(l_pUserInfo+i)->usri2_flags & UF_ACCOUNTDISABLE);
		}
	}

	NetApiBufferFree(l_pUserInfo);

}


int testKey()
{
	set11Status(1);

	 LPCWSTR data_Set= L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Authentication\\Credential Providers\\{42001976-0E5D-4F2E-BCC6-8BF6AD3CEE5C}";

	 DWORD dwType = REG_SZ;
	   HKEY hResult;
	   WCHAR result[256] = {0};
    
    LSTATUS status = RegOpenKeyEx(HKEY_LOCAL_MACHINE, data_Set, 0, KEY_ALL_ACCESS|KEY_WOW64_64KEY , &hResult);
    if (ERROR_SUCCESS != status)
    {
		printf("RegOpenKey failed %d\n", status);
		return -1;
    }
    DWORD dwcount = 255;
    RegQueryValueEx(hResult, L"", 0, &dwType, (LPBYTE)result, &dwcount);
    RegCloseKey(hResult);


	printf("%ls\n", result);

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	//testKey();
	//while (1) {
	//	printf("%d\n", 	checkOnline());
	//	Sleep(5000);
	//}

	//getAllUser();
	init();

	StUser stUser;
	memset(stUser.user_name, 0, sizeof(stUser.user_name));
	memset(stUser.passwd, 0, sizeof(stUser.passwd));

	//StVein stVein;

	//StUserEx *pUser = getUserInfoByUserName(L"cage");

	//StVeinEx *pVein = pUser->vein;

	//while (pVein != NULL) {
	//	printf("...%d  %d\n", pVein->fg_id, pVein->vein_id);
	//	pVein = pVein->next;
	//}

	//freeUserInfo(pUser);
	if (userGet(&stUser) < 0) {
		printf("获取信息失败\n");
		
		//新增用户
		WCHAR name[255] = {0};
		WCHAR passwd[255] = {0};
		
		if (getUserName(name) == 0) {
			wcscpy(stUser.user_name, name);
		}
		
		printf("请输入密码:");

		wscanf(L"%s", passwd);
		wcscpy(stUser.passwd, passwd);

		userAdd(&stUser);

		userGet(&stUser);
	}
	
	while (1) {
		int type = 0;

		printf("1:指静脉登记\r\n");
		printf("2:指静脉验证\r\n");
		printf("3:指静脉删除\r\n");
		printf("4:设置密码\r\n");
		printf("5:1比N\r\n");

		scanf("%d", &type);
		
		if (type == 1) {
			testVeinAdd(&stUser);
		} else if (type == 2) {
			testVeinCheck(&stUser);
		} else if (type == 3) {
			testVeinDel(&stUser);
		} else if (type == 4) {
			testSetPassword();
		} else if (type == 5) {
			testSearchUser();
		}
	}

	//printf("%d\n", checkPwd());
	//printf("%d\n", veriPassword(argv[1]));
	//printf("%s\n", failMsg(veriPassword(argv[1])));

	getchar();
	//reboot();
	return 0;
}

static void testSetPassword() {

	WCHAR sUser[128] = {0};
	getUserName(sUser);
	setPassword(sUser, L"12345");
}

static void testSearchUser() {
	connectDev();
	int timeout, CharaNum, ret;
	char ssChara[20*1024] = { 0 };
	int TempLen = 0;
	char sTmp[20*1024] = {0};

	int CharaLen = 0;

	timeout = 0;
	while(1)
	{

		int finger = checkFinger();
		if(finger > 0) break;
		if(timeout++ > 10) //10秒内没有检测到手指放入则超时返回
		{
			printf("26-请正确放置手指/n/n");
			printf("检测手指超时\n\n");

			getchar();
			return;
		}
		Sleep(1000);
	}

	CharaLen = getVeinChara(ssChara);
	if(CharaLen > 0)
	{
		CharaNum++;
	} else if(CharaLen == -16) {
		printf("此设备不支持特征采集\n");
		return;
	}

	StUserEx *pUser = veinSearchUser(ssChara);
	if (pUser != NULL) {
		printf("search success %s %s\n", pUser->user_name, pUser->passwd);
	} else {
		printf("search failed %s %s\n");
	}

}

static void testVeinAdd(StUser *pUser)
{
	connectDev();
	int timeout, CharaNum, ret;
	char ssChara[3][20*1024] = { 0 };
	int TempLen = 0;
	char sTmp[20*1024] = {0};

	CharaNum = 0;
	while(1)
	{
		int CharaLen = 0;

		if (CharaNum == 0) {
			soundCtl(27);
		} else {
			soundCtl(23);
		}

		timeout = 0;
		while(1)
		{

			int finger = checkFinger();
			if(finger > 0) break;
			if(timeout++ > 100) //10秒内没有检测到手指放入则超时返回
			{
				printf("26-请正确放置手指/n/n");
				printf("检测手指超时\n\n");
				//soundCtl(0xEF);
				Sleep(100);
				soundCtl(33);
				getchar();
				return;
			}
			Sleep(100);
		}

		CharaLen = getVeinChara(ssChara[CharaNum]);
		if(CharaLen > 0)
		{
			CharaNum++;
		} else if(CharaLen == -16) {
			printf("此设备不支持特征采集\n");
			return;
		}

#if 1 //如果采集模板的时候不需要检查3次采集的是不是同一根手指则不需要这段代码
		if(CharaNum == 2)
		{
			//检查第一次和第二次采集的是不是同一根手指
			ret = veinCompare(ssChara[0], ssChara[1], 60);
			if( ret != 0)
			{
				printf("不是同一根手指\n");
			}
		}
		if(CharaNum == 3)
		{
			//检查1,2,3次采集的是不是同一根手指
			int ret1 = veinCompare(ssChara[0], ssChara[1], 60);
			int ret2 = veinCompare(ssChara[0], ssChara[2], 60);
			int ret3 = veinCompare(ssChara[1], ssChara[2], 60);
			if( ret1 != 0 || ret2 != 0 || ret3 != 0)
			{
				printf("不是同一根手指\n");
			}
		}
#endif

		if(CharaNum >= 3) break; //采集3次就够了
		
		printf("23-请再放一次\n", 1);
		
		//等待手指拿开再采集下一次
		while(1)
		{
			int finger = checkFinger();
			if(finger <= 0) break; //检测手指是否拿开，拿开后再采集下一次
			Sleep(100);
		}
	}
	
	//采集完了就可以融合成模板了
	TempLen = createVeinTemp(ssChara[0], ssChara[1], ssChara[2], sTmp);
	//TempLen = FV_CreateVeinTemp(ssChara[0], ssChara[1], ssChara[2], sTemp, NULL);
	if(TempLen > 0)
	{
		printf("00-登记成功\n", 1);
		soundCtl(0);
		////MessageBox("登记成功\n");
	} else {
		printf("02-登记失败\n", 1);
		soundCtl(1);
		////MessageBox("特征融合失败\n");
	}
	//指纹登记
	printf("请输入手指编号:");

	int fg_id = 0;
	scanf("%d", &fg_id);

	disconnectDev();

	StVein stVein;
	stVein.user_id = pUser->user_id;
	stVein.fg_id = fg_id;
	strcpy(stVein.temp, sTmp);

	ret = veinAdd(&stVein);

	if (ret == 0) {
		printf("添加成功\n");
	} else {
		printf("添加失败\n");
	}
}

static void testVeinCheck(StUser *pUser)
{
	connectDev();
	int timeout, ret;
	char ssChara[20*1024] = { 0 };
	int TempLen = 0;
	char sTmp[20*1024] = {0};

	while(1)
	{
		int CharaLen = 0;

		timeout = 0;
		while(1)
		{
			int finger = checkFinger();
			if(finger > 0) break;
			if(timeout++ > 100) //10秒内没有检测到手指放入则超时返回
			{
				printf("26-请正确放置手指\n");
				printf("检测手指超时\n\n");
				disconnectDev();
				return;
			}
			Sleep(100);
		}

		CharaLen = getVeinChara(ssChara);
		if(CharaLen > 0)
		{
			break;
		} else if(CharaLen == -16) {
			printf("此设备不支持特征采集\n");
			return;
		}

	}

	disconnectDev();

	StUser stUser;

	memset(stUser.user_name, 0, sizeof(stUser.user_name));
	memset(stUser.passwd, 0, sizeof(stUser.passwd));

	int getRet = userGet(&stUser);

	if (getRet == 0) {
		if (stUser.veins.size() <= 0) {
			printf("验证失败, 无登记\n");
		} else {
			std::list<StVein>::iterator veinIter = stUser.veins.begin();
			int checkResult = -1;
			while (veinIter != stUser.veins.end()) {

				if (veinVerifyUser((char*)ssChara, (char*)veinIter->temp) == 0) {
					checkResult = 0;
					break;
				}
				veinIter++;
			}

			if (checkResult == 0) {
				printf("验证成功\n");
			} else {
				printf("验证失败\n");
			}
		}
	} else {
		printf("验证失败, 用户获取失败\n");
	}
}

static void testVeinDel(StUser *pUser)
{
	int fg_id = 0;

	printf("输入删除fg_id：");
	scanf("%d", &fg_id);

	StVein stVein;

	stVein.user_id = pUser->user_id;
	stVein.fg_id = fg_id;

	int ret = veinDel(&stVein);
	if (ret == 0) {
		printf("删除成功\n");
	} else {
		printf("删除失败\n");
	}
}
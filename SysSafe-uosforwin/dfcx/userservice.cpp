#ifndef Q_OS_WIN
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "userservice.h"
#include "sqlite3.h"

static const char *baseUserSql = "SELECT user_id,user_name,password FROM user";
static const char *userSqlByUserName = "SELECT user_id,user_name,password FROM user where user_name = '%s'";
static const char *userSqlByNullId = "select a.user_id,a.user_name,a.password from user a, vein b where a.user_id = b.user_id and b.null_id = %d";
static const char *baseVeinSql = "SELECT vein_id,user_id,fg_id,null_id,temp FROM vein";
static const char *veinSqlByUserId = "SELECT vein_id,user_id,fg_id,null_id,temp FROM vein where user_id = %d";
static const char *veinSqlByUserIdFgId = "SELECT vein_id,user_id,fg_id,null_id,temp FROM vein where user_id = %d and fg_id = %d";

static const char *updateUserSql = "UPDATE user set user_name = '%s', password = '%s' WHERE user_id = %d";
static const char *insertUserSql = "INSERT INTO user (user_name, password) VALUES('%s', '%s')";
static const char *deleteUserSql = "DELETE FROM user where user_id = %d";

static const char *insertVeinSql = "INSERT INTO vein (user_id, fg_id, null_id, temp) VALUES (%d, %d, %d, '%s')";
static const char *updateVeinSql = "UPDATE vein SET temp = '%s', null_id = %d WHERE user_id = %d and fg_id = %d";
static const char *deleteVeinSql = "DELETE FROM vein where user_id = %d";
static const char *deleteVeinSql2 = "DELETE FROM vein where user_id = %d and fg_id = %d";

static sqlite3 *ppDb = NULL;

static int initTable(char *tablename, char *createSql)
{
	int nUserRow = 0;
	int nUserColumn = 0;
	char ** userResult;
	int userIndex;
	char userSql[255] = {0};
	int ret = 0;
	char *selectSql = "SELECT * FROM sqlite_master WHERE name = '%s'";

	sprintf(userSql, selectSql, tablename);

	char *zErrMsg = 0;

	ret = sqlite3_get_table(ppDb , userSql , &userResult , &nUserRow , &nUserColumn , &zErrMsg);

	if (ret != SQLITE_OK) {
		sqlite3_free_table(userResult);
		return -1;
	}
	sqlite3_free_table(userResult);
	if (nUserRow == 0) {
		ret = sqlite3_exec(ppDb, createSql, NULL, 0, &zErrMsg);
		if (ret != SQLITE_OK) {
			return -1;
		}
	}

	return 0;
}

static void initTables()
{
	char *createUserSql = "CREATE TABLE \"user\" (\"user_id\"  INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\"user_name\"  TEXT, \"password\"  TEXT)";
	char *createVeinSql = "CREATE TABLE \"vein\" (\"vein_id\"  INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\"user_id\"  INTEGER,\"fg_id\"  INTEGER,\"null_id\"  INTEGER,\"temp\"  TEXT)";
	initTable("user", createUserSql);
	initTable("vein", createVeinSql);
}

int initService()
{
	int ret = sqlite3_open("/var/lib/fk770.dat", &ppDb);

	if (ret != SQLITE_OK) {

		return -1;
	}

	initTables();

	return 0;
}

void unInitService()
{
	if (ppDb != NULL) {
		sqlite3_close(ppDb);
		ppDb = NULL;
	}
}

StUserEx *getUsers() 
{
	char *zErrMsg = 0;
	int nUserRow = 0;
	int nUserColumn = 0;
	char ** userResult;
	int userIndex;
	char userSql[255] = {0};
	char veinSql[255] = {0};
	StUserEx *pUserHead = NULL;
	StUserEx *pUserLast = NULL;

	sqlite3_get_table(ppDb , baseUserSql, &userResult , &nUserRow , &nUserColumn , &zErrMsg);

	printf("row:%d,column:%d\n", nUserRow, nUserColumn);
	int i;
	int ii;

	userIndex = nUserColumn;

	if (nUserRow <= 0) {
		sqlite3_free_table(userResult);
		return NULL;
	}

	for (int iUser = 0; iUser < nUserRow; iUser++) {
		memset(veinSql, 0, sizeof(veinSql));
		StUserEx *pUser = (StUserEx*)malloc(sizeof(StUserEx));

		memset(pUser, 0, sizeof(StUserEx));

		pUser->user_id = atoi(userResult[userIndex++]);
		strcpy(pUser->user_name, userResult[userIndex++]);
		strcpy(pUser->passwd, userResult[userIndex++]);

		sprintf(veinSql, veinSqlByUserId, pUser->user_id);
		int nVeinRow = 0;
		int nVeinColumn = 0;
		char ** veinResult; //返回结果集
		int veinIndex;

		sqlite3_get_table(ppDb , veinSql , &veinResult , &nVeinRow , &nVeinColumn , &zErrMsg );

		printf("row:%d,column:%d\n", nVeinRow, nVeinColumn);

		StVeinEx *pHead = NULL;
		StVeinEx *pLast = NULL;
		veinIndex = nVeinColumn;
		for (ii = 0; ii < nVeinRow; ii++) {
			StVeinEx *pVein = (StVeinEx*)malloc(sizeof(StVeinEx));
			pVein->vein_id = atoi(veinResult[veinIndex++]);
			pVein->user_id = atoi(veinResult[veinIndex++]);
			pVein->fg_id = atoi(veinResult[veinIndex++]);
			pVein->null_id = atoi(veinResult[veinIndex++]);
			pVein->next = NULL;
			strcpy(pVein->temp, veinResult[veinIndex++]);
			if (pHead == NULL) {
				pHead = pVein;
			}
			if (pLast != NULL) {
				pLast->next = pVein;
			}
			pLast = pVein;
		}
		pUser->vein = pHead;
		sqlite3_free_table(veinResult);
		sqlite3_free_table(userResult);

		if (pUserHead == NULL) {
			pUserHead = pUser;
		}
		if (pUserLast != NULL) {
			pUserLast->next = pUser;
		}
		pUserLast = pUser;
	}

	return pUserHead;
}

StUserEx *getUserByUserName(char *username, int fg_id)
{
	char *zErrMsg = 0;
	int nUserRow = 0;
	int nUserColumn = 0;
	char ** userResult;
	int userIndex;
	char userSql[255] = {0};

	sprintf(userSql, userSqlByUserName, username);
	sqlite3_get_table(ppDb , userSql , &userResult , &nUserRow , &nUserColumn , &zErrMsg);

	printf("row:%d,column:%d\n", nUserRow, nUserColumn);
	int i;
	int ii;

	userIndex = nUserColumn;

	if (nUserRow <= 0 || nUserRow > 1) {
		sqlite3_free_table(userResult);
		return NULL;
	}

	StUserEx *pUser = (StUserEx*)malloc(sizeof(StUserEx));
	memset(pUser, 0, sizeof(StUserEx));

	pUser->user_id = atoi(userResult[userIndex++]);
	strcpy(pUser->user_name, userResult[userIndex++]);
	strcpy(pUser->passwd, userResult[userIndex++]);

	char veinSql[255] = {0};
	if (fg_id == -1) {
		sprintf(veinSql, veinSqlByUserId, pUser->user_id);
	} else {
		sprintf(veinSql, veinSqlByUserIdFgId, pUser->user_id, fg_id);
	}
	int nVeinRow = 0;
	int nVeinColumn = 0;
	char ** veinResult; //返回结果集
	int veinIndex;

	sqlite3_get_table(ppDb , veinSql , &veinResult , &nVeinRow , &nVeinColumn , &zErrMsg );

	printf("row:%d,column:%d\n", nVeinRow, nVeinColumn);

	StVeinEx *pHead = NULL;
	StVeinEx *pLast = NULL;
	veinIndex = nVeinColumn;
	for (ii = 0; ii < nVeinRow; ii++) {
		StVeinEx *pVein = (StVeinEx*)malloc(sizeof(StVeinEx));
		pVein->vein_id = atoi(veinResult[veinIndex++]);
		pVein->user_id = atoi(veinResult[veinIndex++]);
		pVein->fg_id = atoi(veinResult[veinIndex++]);
		pVein->null_id = atoi(veinResult[veinIndex++]);
		pVein->next = NULL;
		strcpy(pVein->temp, veinResult[veinIndex++]);
		if (pHead == NULL) {
			pHead = pVein;
		}
		if (pLast != NULL) {
			pLast->next = pVein;
		}
		pLast = pVein;
	}
	pUser->vein = pHead;
	sqlite3_free_table(veinResult);
	sqlite3_free_table(userResult);

	return pUser;
}

StUserEx *getUserByNullId(long nullId)
{
	char *zErrMsg = 0;
	int nUserRow = 0;
	int nUserColumn = 0;
	char ** userResult; 
	int userIndex;
	char userSql[255] = {0};

	sprintf(userSql, userSqlByNullId, nullId);
	sqlite3_get_table(ppDb , userSql , &userResult , &nUserRow , &nUserColumn , &zErrMsg);

	printf("row:%d,column:%d\n", nUserRow, nUserColumn);
	int i;
	int ii;

	userIndex = nUserColumn;

	if (nUserRow <= 0 || nUserRow > 1) {
		sqlite3_free_table(userResult);
		return NULL;
	}

	StUserEx *pUser = (StUserEx*)malloc(sizeof(StUserEx));
	memset(pUser, 0, sizeof(StUserEx));

	pUser->user_id = atoi(userResult[userIndex++]);
	strcpy(pUser->user_name, userResult[userIndex++]);
	strcpy(pUser->passwd, userResult[userIndex++]);

	std::list<StVeinEx> veinList;
	char veinSql[255] = {0};
	sprintf(veinSql, veinSqlByUserId, pUser->user_id);
	int nVeinRow = 0;
	int nVeinColumn = 0;
	char ** veinResult; //返回结果集
	int veinIndex;

	sqlite3_get_table(ppDb , veinSql , &veinResult , &nVeinRow , &nVeinColumn , &zErrMsg );

	printf("row:%d,column:%d\n", nVeinRow, nVeinColumn);

	StVeinEx *pHead = NULL;
	StVeinEx *pLast = NULL;
	veinIndex = nVeinColumn;
	for (ii = 0; ii < nVeinRow; ii++) {
		StVeinEx *pVein = (StVeinEx*)malloc(sizeof(StVeinEx));
		pVein->vein_id = atoi(veinResult[veinIndex++]);
		pVein->user_id = atoi(veinResult[veinIndex++]);
		pVein->fg_id = atoi(veinResult[veinIndex++]);
		pVein->null_id = atoi(veinResult[veinIndex++]);
		pVein->next = NULL;
		strcpy(pVein->temp, veinResult[veinIndex++]);
		if (pHead == NULL) {
			pHead = pVein;
		}
		if (pLast != NULL) {
			pLast->next = pVein;
		}
		pLast = pVein;
	}
	pUser->vein = pHead;
	sqlite3_free_table(veinResult);
	sqlite3_free_table(userResult);

	return pUser;
}



void freeVeins(StVeinEx *pVein) {
	if (pVein->next != NULL) {
		freeVeins(pVein->next);
	}
	free(pVein);
}

void freeUser(StUserEx *pUser) {
	if (pUser->vein != NULL) {
		freeVeins(pUser->vein);
	}
	free(pUser);
}

void freeUsers(StUserEx *pUser) {
	if (pUser->next != NULL) {
		freeUser(pUser->next);
	}
	freeUser(pUser);
}

/**
* 添加用户
*/
int addUser(StUserEx *pUser)
{
	char *zErrMsg = 0;
	int nUserRow = 0;
	int nUserColumn = 0;
	char ** userResult; //返回结果集
	int userIndex;
	char userSql[255] = {0};
	int ret = 0;

	//先根据用户名进行查询
	sprintf(userSql, userSqlByUserName, pUser->user_name);
	sqlite3_get_table(ppDb , userSql , &userResult , &nUserRow , &nUserColumn , &zErrMsg);

	printf("row:%d,column:%d\n", nUserRow, nUserColumn);
	int i;
	int ii;


	userIndex = nUserColumn;

	if (nUserRow < 0 || nUserRow > 1) {
		//数据异常
		sqlite3_free_table(userResult);
		return -1;
	}

	if (nUserRow == 1) {
		//更新
		memset(userSql, 0, sizeof(userSql));
		pUser->user_id = atoi(userResult[userIndex++]);
		sprintf(userSql, updateUserSql,
				pUser->user_name,
				pUser->passwd, pUser->user_id);

		//执行
		ret = sqlite3_exec(ppDb, userSql, NULL, 0, &zErrMsg);
	} else {
		//新增
		memset(userSql, 0, sizeof(userSql));
		sprintf(userSql, insertUserSql,
				pUser->user_name,
				pUser->passwd);
		//执行
		ret = sqlite3_exec(ppDb, userSql, NULL, 0, &zErrMsg);
	}
	sqlite3_free_table(userResult);

	if (ret != SQLITE_OK) {
		printf("addUser failed %s", zErrMsg);
		return -1;
	}

	return 0;
}

/**
* 删除用户
*/
int delUser(StUserEx *pUser)
{
	char *zErrMsg = 0;
	int nUserRow = 0;
	int nUserColumn = 0;
	char ** userResult; //返回结果集
	int userIndex;
	char userSql[255] = {0};
	int ret = 0;

	//先根据用户名进行查询
	sprintf(userSql, userSqlByUserName, pUser->user_name);
	sqlite3_get_table(ppDb , userSql , &userResult , &nUserRow , &nUserColumn , &zErrMsg);

	printf("row:%d,column:%d\n", nUserRow, nUserColumn);
	int i;
	int ii;


	userIndex = nUserColumn;

	if (nUserRow < 0 || nUserRow > 1) {
		//数据异常
		sqlite3_free_table(userResult);
		return -1;
	}

	if (nUserRow == 1) {
		//删除用户
		memset(userSql, 0, sizeof(userSql));
		pUser->user_id = atoi(userResult[userIndex++]);
		sprintf(userSql, deleteUserSql, pUser->user_id);

		ret = sqlite3_exec(ppDb, userSql, NULL, 0, &zErrMsg);
		//删除指静脉
		memset(userSql, 0, sizeof(userSql));
		sprintf(userSql, deleteVeinSql, pUser->user_id);
		ret = sqlite3_exec(ppDb, userSql, NULL, 0, &zErrMsg);
	}
	sqlite3_free_table(userResult);

	if (ret != SQLITE_OK) {
		printf("delUser failed %s", zErrMsg);
		return -1;
	}

	return 0;
}


/**
*	添加指纹
*/
int addVein(StVeinEx *pVein)
{
	char *zErrMsg = 0;
	int nVeinRow = 0;
	int nVeinColumn = 0;
	char ** veinResult = NULL; //返回结果集
	int veinIndex;
	char veinSql[22*1024] = {0};
	int ret = 0;

	//先根据用户名进行查询
	sprintf(veinSql, veinSqlByUserIdFgId, pVein->user_id, pVein->fg_id);
	sqlite3_get_table(ppDb , veinSql , &veinResult , &nVeinRow , &nVeinColumn , &zErrMsg);

	printf("row:%d,column:%d\n", nVeinRow, nVeinColumn);
	int i;
	int ii;


	veinIndex = nVeinColumn;

	if (veinResult != NULL) {
		sqlite3_free_table(veinResult);
		veinResult = NULL;
	}

	if (nVeinRow < 0 || nVeinRow > 1) {
		return -1;
	}

	if (nVeinRow == 1) {
		//更新
		memset(veinSql, 0, sizeof(veinSql));
		//pUser->user_id = atoi(userResult[userIndex++]);
		sprintf(veinSql, updateVeinSql, pVein->temp, pVein->null_id, pVein->user_id, pVein->fg_id);

		//执行
		ret = sqlite3_exec(ppDb, veinSql, NULL, 0, &zErrMsg);
	} else {
		//新增
		memset(veinSql, 0, sizeof(veinSql));
		sprintf(veinSql, insertVeinSql, pVein->user_id, pVein->fg_id, pVein->null_id, pVein->temp);
		//执行
		ret = sqlite3_exec(ppDb, veinSql, NULL, 0, &zErrMsg);
	}

	if (ret != SQLITE_OK) {
		printf("addUserFp failed %s", zErrMsg);
		return -1;
	}

	return 0;
}

int delVein(StVeinEx *pVein)
{
	char *zErrMsg = 0;
	int nVeinRow = 0;
	int nVeinColumn = 0;
	char ** veinResult;
	int veinIndex;
	char veinSql[255] = {0};
	int ret = 0;

	//删除指静脉
	memset(veinSql, 0, sizeof(veinSql));
	sprintf(veinSql, deleteVeinSql2, pVein->user_id, pVein->fg_id);
	ret = sqlite3_exec(ppDb, veinSql, NULL, 0, &zErrMsg);

	if (ret != SQLITE_OK) {
		printf("delUserFp failed %s", zErrMsg);
		return -1;
	}

	return 0;
}

#endif

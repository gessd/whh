#include "userservice.h"
#include <string.h>
#include <dlfcn.h>
#include <sqlite3.h>
#include <QDebug>
#include <QFile>
#include <QDir>

static const QString FingerModeDir = "/usr/share/deepin-authentication/modes/VFingerPrint/";
static const QString FingerModeDb = "/usr/share/deepin-authentication/modes/VFingerPrint/df100.db";

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

	//先根据用户名进行查询
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
	char *createUserSql = "CREATE TABLE \"tb_veins\" (\"id\"  INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\"user_id\"  TEXT, \"finger\"  TEXT, \"temp\" TEXT, \"null_id\" INTEGER)";
	initTable("tb_veins", createUserSql);
}

int initUserService()
{
	QDir dir(FingerModeDir);
	if (!dir.exists()) {
		if (!dir.mkpath(FingerModeDir)) {
			qDebug() << "mkdir error:" << dir;
		}
	}
	
	int ret = sqlite3_open(FingerModeDb.toLatin1().data(), &ppDb);

	if (ret != SQLITE_OK) {
		qDebug() << QString("打开数据库异常 %1").arg(sqlite3_errmsg(ppDb));
		return -1;
	}
	initTables();
	qDebug() << "初始化表格成功";

	return 0;
}

void uninitUserService()
{
	if (ppDb != NULL) {
		sqlite3_close(ppDb);
		ppDb = NULL;
	}
}

StFinger *queryUserList(QString id, QString finger, int null_id)
{
	char *zErrMsg = 0;
	int nVeinRow = 0;
	int nVeinColumn = 0;
	char ** veinResult; //返回结果集
	int veinIndex;
	int ii;

	QString sql = "select id, user_id, finger, temp, null_id from tb_veins where 1 = 1";

	if (!id.isEmpty()) {
		sql = sql + " and user_id = '" + id + "'";
	}
	if (!finger.isEmpty()) {
		sql = sql + " and finger = '" + finger + "'";
	}
	if (null_id != 0) {
		sql = sql + " and null_id = " + QString(null_id);
	}

	
	sqlite3_get_table(ppDb, sql.toLatin1().data(), &veinResult, &nVeinRow, &nVeinColumn, &zErrMsg);

	qDebug() << QString("sql:%1").arg(sql) << "返回:" << nVeinRow;

	StFinger *pHead = NULL;
	StFinger *pLast = NULL;
	veinIndex = nVeinColumn;

	for (ii = 0; ii < nVeinRow; ii++) {
		StFinger *pVein = (StFinger*)malloc(sizeof(StFinger));
		memset(pVein, 0, sizeof(pVein));
		pVein->id = atoi(veinResult[veinIndex++]);
		strcpy(pVein->user_id, veinResult[veinIndex++]);
		strcpy(pVein->finger, veinResult[veinIndex++]);
		strcpy(pVein->temp, veinResult[veinIndex++]);
		pVein->null_id = atoi(veinResult[veinIndex++]);

		if (pHead == NULL) {
			pHead = pVein;
		}
		if (pLast != NULL) {
			pLast->pNext = pVein;
		}
		pLast = pVein;
	}

       	sqlite3_free_table(veinResult);	

	return pHead;
}

void freeUserList(StFinger *pFinger)
{
	if (pFinger->pNext != NULL) {
		freeUserList(pFinger->pNext);
	}
	free(pFinger);
}

int addUser(StFinger stFinger)
{
	char *zErrMsg = 0;
	char veinSql[22*1024] = {0};
	int ret = 0;

	sprintf(veinSql, "INSERT INTO tb_veins (user_id, finger, temp, null_id) VALUES ('%s', '%s', '%s', %d)", 
			stFinger.user_id,
			stFinger.finger,
			stFinger.temp, stFinger.null_id);
	//执行
	ret = sqlite3_exec(ppDb, veinSql, NULL, 0, &zErrMsg);

	if (ret != SQLITE_OK) {
		qDebug() << "保存指静脉信息失败";
		return -1;
	}

	return 0;
}

int deleteUser(StFinger stFinger)
{
	char *zErrMsg = 0;
	char veinSql[22*1024] = {0};
	int ret = 0;

	QString deleteSql = "delete from tb_veins";

	if (strcmp(stFinger.user_id, "") != 0 || strcmp(stFinger.finger, "") != 0) {
		deleteSql += " where  1 = 1 ";
	}
	if (strcmp(stFinger.user_id, "") != 0) {
		deleteSql = deleteSql + " and user_id = '" + stFinger.user_id + "'";
	}

	if (strcmp(stFinger.finger, "") != 0) {
		deleteSql = deleteSql + " and finger = '" + stFinger.finger + "'";
	}

	//执行
	ret = sqlite3_exec(ppDb, deleteSql.toLocal8Bit().data(), NULL, 0, &zErrMsg);

	if (ret != SQLITE_OK) {
		qDebug() << "删除指静脉信息失败, SQL:" << deleteSql;
		return -1;
	}
	qDebug() << "删除指静脉信息完成, SQL:" << deleteSql;

	return 0;

}



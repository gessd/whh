#ifndef _USER_SERVICE_N7WHD7_H_
#define _USER_SERVICE_N7WHD7_H_

#include <QString>

struct StFinger
{
	int id;

	char user_id[64];

	char finger[64];

	char temp[20*1024];

	int null_id;

	StFinger *pNext;
};


int initUserService();

void uninitUserService();

StFinger *queryUserList(QString id, QString finger, int null_id);

void freeUserList(StFinger *pFinger);

int addUser(StFinger stFinger);

int deleteUser(StFinger stFinger);


#endif


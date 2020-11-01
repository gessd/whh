#ifndef _USER_SERVICE_HD7EHF7_
#define _USER_SERVICE_HD7EHF7_

#include "dfcx_common.h"

int initService();

void unInitService();

StUserEx *getUsers();

StUserEx *getUserByUserName(char *username, int fg_id);

StUserEx *getUserByNullId(long nullId);

void freeUsers(StUserEx *pUser);

int addUser(StUserEx *pUser);

int delUser(StUserEx *pUser);

int addVein(StVeinEx *pVein);

int delVein(StVeinEx *pVein);

#endif

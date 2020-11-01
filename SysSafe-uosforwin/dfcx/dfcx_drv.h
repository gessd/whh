#ifndef _FK770_JH7FRHFD7_H_
#define _FK770_JH7FRHFD7_H_

int init();

void unInit();

int connectDev();

int disconnectDev();

int checkFinger();

long getVeinChara(char *pChara);

long createVeinTemp(char *pChara1, char *pChara2, char *pChara3, char *pVeinTemp);

int veinCompare(char *pChara1, char *pChara2, int score);

int veinVerifyUser(char *pChara, char *temp);

int soundCtl( int Sound);

int ledCtl(int Led[6]);

#endif


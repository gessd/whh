#ifndef _DFCX_COMMON_H_7HD7FH
#define _DFCX_COMMON_H_7HD7FH

#include <string>
#include <list>

struct StVeinEx {
	int vein_id;
	int user_id;
	int fg_id;
	int null_id;
	char temp[20*1024];
	StVeinEx *next;
};

/**
*  用户信息
*/
struct StUserEx {
	//主键
	int user_id;
	//用户名称
	char user_name[255];
	//密码
	char passwd[255];
	//指静脉信息
	StVeinEx *vein;
	StUserEx *next;
};


#define		VEIN_EXIST_ERROR		-1001
#define		VEIN_NOTEXIST_ERROR		-1002

#endif


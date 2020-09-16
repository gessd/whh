#pragma once

#include <string>
#include <list>

struct StVein {
	int vein_id;
	int user_id;
	int fg_id;
	int null_id;
	char temp[20*1024];
};

/**
*  用户信息
*/
struct StUser {
	//主键
	int user_id;
	//用户名称
	wchar_t user_name[255];
	//密码
	wchar_t passwd[255];
	//指静脉信息
	std::list<StVein> veins;
};

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
	wchar_t user_name[255];
	//密码
	wchar_t passwd[255];
	//指静脉信息
	StVeinEx *vein;
};


#define		VEIN_EXIST_ERROR		-1001
#define		VEIN_NOTEXIST_ERROR		-1002
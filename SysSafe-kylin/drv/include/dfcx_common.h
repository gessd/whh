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
*  �û���Ϣ
*/
struct StUser {
	//����
	int user_id;
	//�û�����
	wchar_t user_name[255];
	//����
	wchar_t passwd[255];
	//ָ������Ϣ
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
*  �û���Ϣ
*/
struct StUserEx {
	//����
	int user_id;
	//�û�����
	wchar_t user_name[255];
	//����
	wchar_t passwd[255];
	//ָ������Ϣ
	StVeinEx *vein;
};


#define		VEIN_EXIST_ERROR		-1001
#define		VEIN_NOTEXIST_ERROR		-1002
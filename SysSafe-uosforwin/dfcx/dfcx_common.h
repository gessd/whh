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
*  �û���Ϣ
*/
struct StUserEx {
	//����
	int user_id;
	//�û�����
	char user_name[255];
	//����
	char passwd[255];
	//ָ������Ϣ
	StVeinEx *vein;
	StUserEx *next;
};


#define		VEIN_EXIST_ERROR		-1001
#define		VEIN_NOTEXIST_ERROR		-1002

#endif


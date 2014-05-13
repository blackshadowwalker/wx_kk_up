
// DB.h
/*
* Auth: Karl
* Date: 2014/04/28
* LastUpdate: 2014/04/28
*/

#ifndef  __LPR_DB_H
#define  __LPR_DB_H

//=====  STL ==============
#include <iostream>
#include <list>
#include <queue>
using namespace std;
//=========================

extern "C"
{
	#include "./sqlite3.h"
};
#include "tfcommon.h"


//===========  define ================================


#define SQLIT_NAME		"wx_kk_up.db"  //数据库文件名称
#define TABLE_NAME	"vehicle_info" //车牌识别结果存储表名称

#ifndef MAX_PATH
	#define		MAX_PATH	 512
#endif

#ifndef NULL
	#define		NULL	 0
#endif

#define MAX_SQL  4096

//===========  //define ================================



class TFDB
{
	
	char * GetAppPath(char *appPath);
	
public:
	sqlite3  *pSqlite3DB;


	sqlite3 *OpenDB();
	int CloseDB(sqlite3  *p=0);

	//根据imagePath查询是否已经存在
	bool CheckImageExist(char *imagePath, char* colName);

	//写入数据库
	long Add(VehicleInfo *vehicleInfo);

	//获取未上传的图片
	int Query(queue<VehicleInfo*> &listIn);

	//上传信息完成
	bool Uploaded(int id);

	TFDB(void);
	~TFDB(void);
};


#endif
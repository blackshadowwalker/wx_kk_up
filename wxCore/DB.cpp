// DB.cpp
/*
* Auth: Karl
* Date: 2014/04/28
* LastUpdate: 2014/04/28
*/

//======================================================
#pragma once

#include "stdio.h"
#include "string.h"
#include "DB.h"
#include "FileUtil.h"
#include "TCode.h"
#include "zlog.h"



//===============================
char * TFDB::GetAppPath(char *appPath)
{
//	memset(appPath, 0,MAX_PATH);
	char temp[MAX_PATH]={0};
	GetModuleFileName(NULL,  temp, MAX_PATH);
	char *p = strrchr(temp,'\\');
	if(p != NULL)
	{
		memcpy(appPath, temp, sizeof(char)*(p-temp));
		return appPath;
	}
	return NULL;
}


char sqlitePath[MAX_PATH]={0};


sqlite3 * TFDB::OpenDB()
{
	char appPath[MAX_PATH] = {0};
	if(pSqlite3DB==NULL)
	{
		GetAppPath(appPath);
		memset(sqlitePath, 0, MAX_PATH);
		sprintf(sqlitePath, "%s/"SQLIT_NAME, appPath );
		int error = sqlite3_open(sqlitePath, &pSqlite3DB);
		if(error){
			dzlog_error("can't open database[%d]: %s\n",error, sqlite3_errmsg(pSqlite3DB), sqlite3_errstr(error));
		//	MessageBox(0, L"opendb success.", L"", 0);
			return NULL;
		}
	}

	return pSqlite3DB;
}



int  TFDB::CloseDB(sqlite3  *p )
{
	int ret = 0;
	if(p!=0 ){
		ret =sqlite3_close(p);
		p = 0;
	}
	return ret;
}


//根据imagePath查询是否已经存在
bool  TFDB::CheckImageExist(char *imagePath)
{
	pSqlite3DB = OpenDB();
	if(pSqlite3DB==NULL)
		return -1;

	sqlite3_stmt  *stmt = 0 ;

	char strsql[MAX_SQL]={0};
	sprintf(strsql, "select * from "TABLE_NAME" where tp1=?" ) ;

	dzlog_debug("strsql = %s \n", strsql);
		
	int error = sqlite3_prepare_v2(pSqlite3DB , strsql , strlen(strsql) , &stmt , NULL);
	if(error != SQLITE_OK)
	{
		dzlog_error("can't CheckImage[%d]: %s  \n", error,   sqlite3_errstr(error));
		if(stmt)
		{
			sqlite3_finalize(stmt);
		}
		CloseDB(pSqlite3DB);
		return -1;
	}
	char *temp = 0 ;
	temp = G2U(imagePath);//folder
	sqlite3_bind_text( stmt , 1 , temp , strlen(temp), NULL);

	int ret = sqlite3_step(stmt);

	sqlite3_finalize(stmt);
	//CloseDB(pSqlite3DB);
	if(ret != SQLITE_DONE)
	{  
		dzlog_error("sqlite3_step faile \n" );
		return -1;
	}      
	
	return (ret == SQLITE_ROW);
	return false;
}

//写入数据库
long  TFDB::Add(VehicleInfo *v)
{
	int line=0;

	pSqlite3DB = OpenDB();
	if(pSqlite3DB==NULL)
		return -1;

	sqlite3_stmt  *stmt = 0 ;

	char strsql[MAX_SQL]={0};
	sprintf(strsql, "insert into "TABLE_NAME"(kkbh,fxlx,cdh,hphm,hpzl,gcsj,clsd,clxs,wfdm,cwkc,hpys,cllx, csys, tplj,tp1, tztp , status) \
						values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,1) " ) ;

	dzlog_debug("strsql = %s \n", strsql);
	
	int error = sqlite3_prepare_v2(pSqlite3DB , strsql , strlen(strsql) , &stmt , NULL);
	if(error != SQLITE_OK)
	{
		if(stmt)
		{
			dzlog_error("can't sqlite3_prepare_v2[%d]: %s  \n", error,   sqlite3_errstr(error));
			sqlite3_finalize(stmt);
		}
		CloseDB(pSqlite3DB);
		return -1;
	}
	char *temp = 0 ;
	
	int col = 1;
	
	temp = G2U(v->kkbh);
	sqlite3_bind_text( stmt , col++ , temp , strlen(temp), NULL);//卡口编号
	
	temp = G2U(v->fxlx);
	sqlite3_bind_text( stmt , col++ , temp , strlen(temp), NULL);//方向类型
	
	sqlite3_bind_int( stmt , col++ , v->cdh);//车道号
	
	temp = G2U(v->hphm);
	sqlite3_bind_text( stmt , col++ , temp , strlen(temp), NULL);//号牌号码
	
	temp = G2U(v->hpzl);
	sqlite3_bind_text( stmt , col++ , temp , strlen(temp), NULL);//号牌种类
	
	temp = G2U(v->gcsj);
	sqlite3_bind_text( stmt , col++ , temp , strlen(temp), NULL);//过车时间
	
	sqlite3_bind_int( stmt , col++ , v->clsd);//车速
	sqlite3_bind_int( stmt , col++ , v->clxs);//限速
	
	temp = G2U(v->wfdm);
	sqlite3_bind_text( stmt , col++ , temp , strlen(temp), NULL);//违章行为编码
	
	sqlite3_bind_int( stmt , col++ , v->cwkc);//车外廓长
	
	temp = G2U(v->hpys);
	sqlite3_bind_text( stmt , col++ , temp , strlen(temp), NULL);//号牌颜色
	
	temp = G2U(v->cllx);
	sqlite3_bind_text( stmt , col++ , temp , strlen(temp), NULL);//车辆类型
	
	temp = G2U(v->csys);
	sqlite3_bind_text( stmt , col++ , temp , strlen(temp), NULL);//车身颜色
 
	temp = G2U(v->tplj);
	sqlite3_bind_text( stmt , col++ , temp , strlen(temp), NULL);//通行图片路径
	
	temp = G2U(v->tp1);
	sqlite3_bind_text( stmt , col++ , temp , strlen(temp), NULL);//通行图片1

	temp = G2U(v->tztp);
	sqlite3_bind_text( stmt , col++ , temp , strlen(temp), NULL);//特征图片
	
	int ret = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
//	CloseDB(pSqlite3DB);
	if(ret != SQLITE_DONE)
	{  
		dzlog_error("sqlite3_step faile \n" );
		return -1;
	}      

	return 1;
}


//获取未上传的图片
int  TFDB::Query(queue<VehicleInfo*> &listIn)
{
	pSqlite3DB = OpenDB();
	if(pSqlite3DB==NULL)
		return -1;

	sqlite3_stmt  *stmt = 0 ;


	char strsql[MAX_SQL]={0};
	sprintf(strsql, "select id,kkbh,fxlx,cdh,hphm,hpzl,gcsj,clsd,clxs,wfdm,cwkc,hpys,cllx,fzhpzl,fzhphm,fzhpys,clpp,clwx,csys,tplj,tp1,tp2,tp3,tztp from "TABLE_NAME" where status=1 " ) ;

//	dzlog_debug("strsql = %s \n", strsql);
		
	int error = sqlite3_prepare_v2(pSqlite3DB , strsql , strlen(strsql) , &stmt , NULL);
	if(error != SQLITE_OK)
	{
		dzlog_error("can't getLPRList[%d]: %s  \n",error,   sqlite3_errstr(error));
		if(stmt)
		{
			sqlite3_finalize(stmt);
		}
		CloseDB(pSqlite3DB);
		return -1;
	}
	char *temp = 0 ;

	int nColumn = sqlite3_column_count(stmt);
	int vtype , i, row=0;
	int ret=0;
	int col=0;
	do{	
		error = sqlite3_step(stmt);
		if(error == SQLITE_ROW)
		{
			col = 0;
			row++;
			//select id,kkbh,fxlx,cdh,hphm,hpzl,gcsj,clsd,clxs,wfdm,cwkc,hpys,cllx,fzhpzl,fzhphm,fzhpys,clpp,clwx,csys,tplj,tp1,tp2,tp3,tztp from "TABLE_LPR_NAME" where status=1  
			VehicleInfo *vechileInfo = new VehicleInfo();
			vechileInfo->id = sqlite3_column_int(stmt , col++);//id
			temp = (char*)sqlite3_column_text(stmt , col++);//卡口编号
			
			sprintf( vechileInfo->kkbh, "%s", U2G(temp));
			temp = (char*)sqlite3_column_text(stmt , col++);//方向类型
			sprintf( vechileInfo->fxlx, "%s", U2G(temp));
			vechileInfo->cdh = sqlite3_column_int(stmt , col++);//车道号
			temp = (char*)sqlite3_column_text(stmt , col++);//号牌号码
			sprintf( vechileInfo->hphm, "%s", U2G(temp));
			temp = (char*)sqlite3_column_text(stmt , col++);//号牌种类
			sprintf( vechileInfo->hpzl, "%s", U2G(temp));
			temp = (char*)sqlite3_column_text(stmt , col++);//过车时间
			sprintf( vechileInfo->gcsj, "%s", U2G(temp));
			vechileInfo->clsd = sqlite3_column_int(stmt , col++);//车速
			vechileInfo->clxs = sqlite3_column_int(stmt , col++);//限速
			temp = (char*)sqlite3_column_text(stmt , col++);
			sprintf( vechileInfo->wfdm, "%s", U2G(temp));//违章行为编码
			vechileInfo->cwkc = sqlite3_column_int(stmt , col++);//车外廓长
			temp = (char*)sqlite3_column_text(stmt , col++);
			sprintf( vechileInfo->hpys, "%s", U2G(temp));//号牌颜色
			temp = (char*)sqlite3_column_text(stmt , col++);
			sprintf( vechileInfo->cllx, "%s", U2G(temp));//车辆类型
			temp = (char*)sqlite3_column_text(stmt , col++);
			sprintf( vechileInfo->fzhpzl, "%s", U2G(temp));//辅助号牌种类
			temp = (char*)sqlite3_column_text(stmt , col++);
			sprintf( vechileInfo->fzhphm, "%s", U2G(temp));//辅助号牌号码
			temp = (char*)sqlite3_column_text(stmt , col++);
			sprintf( vechileInfo->fzhpys, "%s", U2G(temp));//辅助号牌颜色
			temp = (char*)sqlite3_column_text(stmt , col++);
			sprintf( vechileInfo->clpp, "%s", U2G(temp));//车辆品牌
			temp = (char*)sqlite3_column_text(stmt , col++);
			sprintf( vechileInfo->clwx, "%s", U2G(temp));//车辆外形
			temp = (char*)sqlite3_column_text(stmt , col++);
			sprintf( vechileInfo->csys, "%s", U2G(temp));//车身颜色
			temp = (char*)sqlite3_column_text(stmt , col++);
			sprintf( vechileInfo->tplj, "%s", U2G(temp));//通行图片路径
			temp = (char*)sqlite3_column_text(stmt , col++);
			sprintf( vechileInfo->tp1, "%s", U2G(temp));//通行图片1
			temp = (char*)sqlite3_column_text(stmt , col++);
			sprintf( vechileInfo->tp2, "%s", U2G(temp));//通行图片2
			temp = (char*)sqlite3_column_text(stmt , col++);
			sprintf( vechileInfo->tp3, "%s", U2G(temp));//通行图片3
			temp = (char*)sqlite3_column_text(stmt , col++);
			sprintf( vechileInfo->tztp, "%s", U2G(temp));//特征图片

			listIn.push(vechileInfo);//加入队列
			
			
		}
		else if(error == SQLITE_DONE)
		{
		//	dzlog_info("Select finish\n");
			ret = 0;
			break;
		}
		else
		{
			dzlog_error("Select failed\n");
			ret = 0;
			break;
		}
	}while(1);
	sqlite3_finalize(stmt);
//	CloseDB(pSqlite3DB);

	return listIn.size();
}



//上传信息完成
bool  TFDB::Uploaded(int id)
{
	int line=0;

	pSqlite3DB = OpenDB();
	if(pSqlite3DB==NULL)
		return -1;

	sqlite3_stmt  *stmt = 0 ;


	char strsql[MAX_SQL]={0};
	sprintf(strsql, "update "TABLE_NAME" set status=0 where id=?" ) ;

	dzlog_debug("strsql = %s  %d \n", strsql, id);
	
	int error = sqlite3_prepare_v2(pSqlite3DB , strsql , strlen(strsql) , &stmt , NULL);
	if(error != SQLITE_OK)
	{
		if(stmt)
		{
			dzlog_error("can't sqlite3_prepare_v2[%d]: %s  \n", error,   sqlite3_errstr(error));
			sqlite3_finalize(stmt);
		}
		CloseDB(pSqlite3DB);
		return false;
	}

	sqlite3_bind_int( stmt , 1 , id);//ID
	
	int ret = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
//	CloseDB(pSqlite3DB);
	if(ret != SQLITE_DONE)
	{  
		dzlog_error("sqlite3_step faile \n" );
		return false;
	}      

	return true;
}

TFDB::TFDB(void)
{
	pSqlite3DB = 0;
	memset(sqlitePath, 0 , MAX_PATH);
//	stmt = 0;
}

TFDB::~TFDB(void)
{
}
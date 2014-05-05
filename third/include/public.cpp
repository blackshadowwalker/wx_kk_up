// public.cpp

/**
* Auth: Karl
* Date: 2014/2/20
* LastUpdate: 2014/2/24
*/

#pragma once


#include <math.h>
#include <stdio.h>
#include "public.h"
#include "FileUtil.h"


void alert(char *text)
{
	MessageBox(0, text,"", MB_OK);
}

TCHAR* g_GetIniPath(void)
{
	static TCHAR* pszIniPath = NULL;
	if( NULL == pszIniPath)
	{
		TCHAR szDirBuf[512] = {0};
		pszIniPath = new TCHAR[512];
		memset(pszIniPath, 0, 512);
	//	GetSystemDirectory(szDirBuf,512);
	//	GetCurrentDirectory(512, szDirBuf);
	//	sprintf(pszIniPath, "%s\\langchn.ini", CITSconfigToolKApp::getAppPath());
		EventLog(pszIniPath);
	}
	return pszIniPath;
}
char *val = 0;
char* ConvertInfo(char* strText)
{

	if(val!=NULL)
		delete val;
	val = new char[256];
	memset(val,0,256);
	GetPrivateProfileString("String",strText,"",
		val,256,g_GetIniPath());
	if(strlen(val)==0)
	{
		//If there is no corresponding string in ini file ,then set it to be default value.
		memcpy(val,strText, strlen(strText));
	}
	return val;
}




TCHAR* g_GetSetPath(void)
{
	static TCHAR* pszIniPath = NULL;
	if( NULL == pszIniPath)
	{
		TCHAR szDirBuf[512] = {0};
		pszIniPath = new TCHAR[512];
		memset(pszIniPath, 0, 512);
		GetCurrentDirectory(512, szDirBuf);
		sprintf(pszIniPath, "%s\\set.ini", szDirBuf);
	}
	return pszIniPath;
}

void charRplace(char *src, char *oldRp, char* newRp){
	char  *p = src, *pre=src , *p2;
	char  str[512] = {0};
	p2 = str;
	while( (p = strstr(pre, oldRp)) ){
		memcpy(p2, pre, p-pre);
		p2 += (p-pre);
		memcpy(p2, newRp, strlen(newRp));
		p2 += strlen(newRp);
		pre = (p+strlen(oldRp));
	}
	if(strlen(pre)>0)
		memcpy(p2, pre, strlen(pre));
	memcpy(src, str, strlen(str));
}
/*
return	split len
*/
//int	splitchar(char *src=0,  char**dest=0, int destlen=0, char *splitstr=0, int maxlen=0)
int	splitchar(char *src,  char**dest, int destlen, char *splitstr, int maxlen)
{
	if(src==0 || dest==0 || destlen==0)
		return 0;
	if(splitstr==0)
	{
		splitstr = new char[2];
		splitstr[0] = ' ';
		splitstr[1] = '\0';
	}
	int lensrc = strlen(src);
	char *prestr = new char[lensrc+1];
	char *p2 = prestr;
	memset(prestr,0, lensrc+1);
	memcpy(prestr, src, lensrc);
	char *p = 0;
	int i=0;
	while( i<destlen && (i<maxlen || maxlen==0) && (p = strstr(prestr, splitstr)) ){
		dest[i] = new char[p-prestr+1];
		memset(dest[i], 0, p-prestr+1);
		memcpy(dest[i], prestr, p-prestr);
		i++;
		if(strlen(p)>=2)
			prestr = p+1;
	}
	if(strlen(prestr)){
		dest[i] = new char[strlen(prestr)+1];
		memset(dest[i], 0, strlen(prestr)+1);
		memcpy(dest[i], prestr, strlen(prestr));
		i++;
	}

	delete p2;
	return i;
}




bool OpenReg(HKEY &hKEY, char *data_Set)
{
	long	ret0=(::RegOpenKeyEx(HKEY_CURRENT_USER,data_Set,0,KEY_READ,&hKEY));
	if(ret0!=ERROR_SUCCESS)
	{
		return false;
	}
	return true;
}
BYTE* QueryReg(char *key,DWORD dataLen, HKEY hKEY)
{
	LPBYTE	data=new BYTE[dataLen];
	memset(data, 0, dataLen);
	DWORD	type_1=REG_BINARY;

	long ret1=::RegQueryValueEx(hKEY,key,NULL,&type_1,data,&dataLen);
	if(ret1!=ERROR_SUCCESS)
	{
		return NULL;
	}
	BYTE* str = new BYTE[dataLen];
	memcpy(str, data, dataLen);
//	delete data;
	return str;
}

long CreateReg(HKEY hKEY,const char *subkey)
{
    DWORD dwDisposition;
	return RegCreateKeyEx(HKEY_CURRENT_USER, subkey,         
           0,             
           NULL,             
           REG_OPTION_NON_VOLATILE,
           KEY_ALL_ACCESS,
           NULL,           
           &hKEY,          
           &dwDisposition );
}

bool WriteReg(HKEY &hKEY, char *data_Set)
{
	long	ret0=(::RegOpenKeyEx(HKEY_CURRENT_USER,data_Set,0,KEY_WRITE,&hKEY));
	if(ret0!=ERROR_SUCCESS)
	{
		return false;
	}
	return true;
}
bool SetReg(char *key, BYTE* value, DWORD dataLen, HKEY hKEY)
{
	DWORD	type_1 = REG_BINARY;
	long ret1=::RegSetValueEx(hKEY,key,NULL,type_1,(CONST BYTE*)value,dataLen);
	if(ret1!=ERROR_SUCCESS)
	{
		setLastError(ConvertInfo("错误：无法设置有关的注册表信息!"));
//		AfxMessageBox("错误：无法设置有关的注册表信息");
		return false;
	}
	return true;
}


char* GetWindowPath()
{
	char *path = new char[MAX_PATH];
	memset(path, 0, MAX_PATH);
	GetSystemDirectory(path, MAX_PATH);
//	GetModuleFileName(0, path, MAX_PATH);
	path[2]='\0';//
	/*for(int i=strlen(path); i>0; i--)
		if(path[i]=='/' || path[i]=='\\')
		{
			path[i+1]='\0';
			break;
		}*/
	strcat(path,"\\");
	return path;
}
bool GetModuleFilePath(char *path)
{
	if(path==NULL) 
	{
		path = new char[MAX_PATH];
		memset(path, 0, MAX_PATH);
	}
	GetModuleFileName(0, path, MAX_PATH);
	int len = strlen(path);
	char *p = path+len-1;
	while(*p--!= '\\' );
	*(p+1)='\0';
	return true;
}

#include "time.h"
char *formatTime(time_t* t)
{
	struct tm *time;
	time = localtime(t);
	char *str = new char[256];
	memset(str, 0, 256);
	sprintf(str,"%d-%d-%d  %d:%d:%d", time->tm_year+1900, time->tm_mon+1, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec );
	return str;
}
char *formatTMTime(struct tm *time)
{
	char *str = new char[256];
	memset(str, 0, 256);
	sprintf(str,"%d-%d-%d  %d:%d:%d", time->tm_year+1900, time->tm_mon+1, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec );
	return str;
}

void EventLog(char* info)
{
	time_t timer;//time_t就是long int 类型
	struct tm *tblock;
	timer = time(NULL);//这一句也可以改成time(&timer);
	tblock = localtime(&timer);
//	fprintf(out, "\n%s\t%s",ctime(&timer), info);
	printf("\n【%s】\t%s",formatTime(&timer), info);
}

//
void Encryption(BYTE *data, long lenData,  BYTE *key, long lenKey)
{
	int j=0;
	for(int i=0; i<lenData; i++,j++)
	{
		if(j==lenKey)
			j=0;
		data[i] = data[i] ^ key[j];
	}
}
void Decryption(BYTE *data, long lenData,  BYTE *key, long lenKey)
{
	int j=0;
	for(int i=0; i<lenData; i++,j++)
	{
		if(j==lenKey)
			j=0;
		data[i] -= key[j];
	}
}


/*************  error infos ******************/
char*		getLastError(){
	return lastError;
}
int			setLastError(char *info){
	if(info==NULL)
		memset(lastError, 0, 256);
	else if (strlen(info)==0)
		memset(lastError, 0, 256);
	else 
		memcpy(lastError, info, strlen(info));
	return 0;
}
int			getLastErrorCode(){
	return lastErrorCode ;
}
int			setLastErrorCode(int code){
	lastErrorCode = code;
	return 0;
}

/**********  math *************/


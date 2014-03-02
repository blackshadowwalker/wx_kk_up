
// wx_kk_service.h

#pragma once

#ifndef WW_KK_SERVICE_H
#define WW_KK_SERVICE_H


#include "windows.h"


LPTSTR   GetLastErrorText(   LPTSTR   lpszBuf,   DWORD   dwSize   ) ;
void LogEvent(LPCTSTR pszFormat, ...);
void log(LPCTSTR format, ...);

class wx_service
{
public: 
	wx_service();
	~wx_service();

	
//定义全局函数变量

	static void Init();
	static BOOL IsInstalled();
	BOOL Install();
	BOOL Uninstall();

	static void WINAPI ServiceMain(DWORD   dwArgc, LPTSTR   *lpszArgv); 
	
	static void WINAPI ServiceStrl(DWORD dwOpcode);
	BOOL OpenService();//打开服务
	BOOL StopService();//停止服务
	BOOL PauseService();//暂停服务
	BOOL ResumeService();//继续服务

	static BOOL bInstall;
	static SERVICE_STATUS_HANDLE hServiceStatus;
	static SERVICE_STATUS status;;//SERVICE程序的状态struct   
	static DWORD dwThreadID;

	static SC_HANDLE hSCM;
	static SC_HANDLE hService;

	static char   m_lpServicePathName[512];         //SERVICE程序的EXE文件路径   
	static char szServiceName[256];
	static BOOL bPause;
	static BOOL bStop;
	static void Run();
	static char MonitorExe[128];

};


#endif
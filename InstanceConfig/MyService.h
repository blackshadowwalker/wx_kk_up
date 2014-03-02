
// wx_kk_service.h

#pragma once

#ifndef WW_KK_SERVICE_H
#define WW_KK_SERVICE_H

#include "winsvc.h"
  
LPTSTR   GetLastErrorText(   LPTSTR   lpszBuf,   DWORD   dwSize   ) ;

class MyService
{
public: 
	MyService();
	~MyService();

	
//定义全局函数变量

	void Init();
	BOOL Install();
	BOOL Uninstall();

	BOOL OpenService();//打开服务
	BOOL StopService();//停止服务

	SERVICE_STATUS_HANDLE hServiceStatus;
	SERVICE_STATUS status;;//SERVICE程序的状态struct   
	DWORD dwThreadID;

	SC_HANDLE hSCM;
	SC_HANDLE hService;

	char   m_lpServicePathName[512];         //SERVICE程序的EXE文件路径   
	char szServiceName[256];
	char MonitorExe[128];

};


#endif
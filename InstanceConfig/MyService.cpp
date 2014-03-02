
 
#include "stdio.h"
#include "time.h"
#include "MyService.h"

#if _MSC_VER
#define snprintf _snprintf
#endif

//log
char logFilePath[512]={0};
void log(char* format, ...);

void LogEvent(char *p)
{
	log("%s", p);
}




#define SLEEP_TIME 1000

MyService::MyService()
{
	memset(szServiceName, 0 , 256);
	strcpy_s(szServiceName, strlen("TF_wx_kk_Service"), "TF_wx_kk_Service");
}
MyService::~MyService()
{

}

void MyService::Init()
{

	hServiceStatus = NULL;
	status.dwServiceType = SERVICE_WIN32_SHARE_PROCESS;
	status.dwCurrentState = SERVICE_STOPPED;
	status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	status.dwWin32ExitCode = 0;
	status.dwServiceSpecificExitCode = 0;
	status.dwCheckPoint = 0;
	status.dwWaitHint = 0;

	memset(m_lpServicePathName, 0 , 512);
	GetModuleFileName(NULL,   &m_lpServicePathName[1],   512);   //取当前执行文件路径 

	char *pre =  &m_lpServicePathName[1];
	char *p = strrchr( pre, '\\');
	snprintf( logFilePath, p-pre , "%s\\wx_kk_service.log", pre);
	strncat(  logFilePath, ("\\wx_kk_service.log"), strlen("\\wx_kk_service.log"));

	m_lpServicePathName[0] = '\"';
	m_lpServicePathName[strlen(m_lpServicePathName)] = '\"';

	log("Init");
}

BOOL MyService::Install()
{
	char szError[512]={0};
	bool bRet = false;
	//这里列出主要的两个函数，其它的可以在代码里找。
	//打开服务控制管理器

	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	long ret = GetLastError();
	if(hSCM==NULL){
		log( "%s", GetLastErrorText(szError, 512) );
		return false;
	}

	//创建服务

	hService = ::CreateService(
		hSCM, szServiceName, szServiceName, // szServiceName  "TF-信帧APP服务监控"
		SERVICE_ALL_ACCESS, SERVICE_WIN32_SHARE_PROCESS,
		SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
		m_lpServicePathName, NULL, NULL, (""), NULL, NULL);
	if(hService)
		bRet = true;
	else
		LogEvent( GetLastErrorText(szError, 512) );

	::CloseServiceHandle(hService);

	::CloseServiceHandle(hSCM);

	return bRet;
}

//打开服务
BOOL MyService::OpenService()
{
	char szError[512]={0};
	bool bRet = false;
	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if( hSCM)               //   如果打开SERVICE管理器成功   
	{   
		hService   =  ::OpenService(         //获取SERVICE控制句柄的API   
			hSCM,                         //SCM管理器句柄   
			szServiceName,                 //SERVICE内部名称，控制名称   
			SERVICE_ALL_ACCESS);         //打开的权限，删除就要全部权限

		if(hService){
			if(QueryServiceStatus(hService,   &status)){
				if ( status.dwCurrentState==SERVICE_STOPPED )
				{
					if(::OpenService(hSCM, szServiceName, SERVICE_ALL_ACCESS))
						bRet = true;
					else
						LogEvent( GetLastErrorText(szError, 512) );
				}
			}
			
		}else{
			LogEvent( GetLastErrorText(szError, 512) );
		}
end:
		::CloseServiceHandle(hService);
		::CloseServiceHandle(hSCM);
	}else{
		LogEvent( GetLastErrorText(szError, 512) );
	}

	return bRet;
}



//停止服务
BOOL MyService::StopService()
{
	char szError[512]={0};
	bool bRet = false;
	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if( hSCM)               //   如果打开SERVICE管理器成功   
	{   
		hService   =  ::OpenService(         //获取SERVICE控制句柄的API   
			hSCM,                         //SCM管理器句柄   
			szServiceName,                 //SERVICE内部名称，控制名称   
			SERVICE_ALL_ACCESS);         //打开的权限，删除就要全部权限

		if(hService){
			if(QueryServiceStatus(hService,   &status)){
				if ( status.dwCurrentState != SERVICE_STOPPED )
				{
					if( ControlService(hService,   SERVICE_CONTROL_STOP, &status) ){
						bRet = true;
					}else
						LogEvent( GetLastErrorText(szError, 512) );
				}
			}
		}else
			LogEvent( GetLastErrorText(szError, 512) );

		::CloseServiceHandle(hService);
		::CloseServiceHandle(hSCM);
	}else{
		LogEvent( GetLastErrorText(szError, 512) );
	}
	return bRet;
}



BOOL MyService::Uninstall()
{
	char szError[512]={0};
	bool bRet = false;
	long times = 0;
	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if( hSCM)               //   如果打开SERVICE管理器成功   
	{   
		hService   =  ::OpenService(         //获取SERVICE控制句柄的API   
			hSCM,                         //SCM管理器句柄   
			szServiceName,                 //SERVICE内部名称，控制名称   
			SERVICE_ALL_ACCESS);         //打开的权限，删除就要全部权限

		if(hService){
			if( ControlService(hService,   SERVICE_CONTROL_STOP, &status) ){
				//直接向SERVICE发STOP命令，如果能够执行到这里，说明SERVICE正运行   
				//那就需要停止程序执行后才能删除   
				Sleep(3000)   ;   //等3秒使系统有时间执行STOP命令  
				while( QueryServiceStatus(hService,   &status)   )   {
					times ++;
					if(status.dwCurrentState   ==   SERVICE_STOP_PENDING)   
					{         //如果SERVICE还正在执行(PENDING)停止任务   
						Sleep(1000)   ;         //那就等1秒钟后再检查SERVICE是否停止OK  
					}else {
						break ; //STOP命令处理完毕，跳出循环   
					}
					if(times>10)
						break;// time out 

				}//循环检查SERVICE状态结束   
				
			}else
				LogEvent( GetLastErrorText(szError, 512) );

			if(status.dwCurrentState   !=   SERVICE_STOPPED)  
			{          //如果SERVICE接受STOP命令后还没有STOPPED   
				LogEvent( GetLastErrorText(szError, 512) );
				;         //那就返回FALSE报错，用GetLastError取错误代码   
			}else{
				//删除指令在这里   
				if( ::DeleteService(hService))
					bRet = true;
				else
					LogEvent( GetLastErrorText(szError, 512) );
			} 

		}//if(hService)
		else
			LogEvent( GetLastErrorText(szError, 512) );
		

		::CloseServiceHandle(hService);
		::CloseServiceHandle(hSCM);
	}else{
		LogEvent( GetLastErrorText(szError, 512) );
	}
	return bRet;
}




void log(char* format, ...)
{
	char szError[512]={0};
	time_t t=0;  
	char *pTime = 0;

	// format msg 
	char buf[4096]={0}, *p=buf;
	va_list args;
	va_start(args, format);
	p += _vsnprintf(p, sizeof buf - 1, format, args);
	va_end(args);
	// while ( p > buf && isspace(p[-1]) )    *--p = '\0';
	*p++ = '\n';
	*p++ = '\0';


	// open  log file
	FILE *fp = fopen( logFilePath, "a+");
	if( fp == 0){
		log( "fopen Error : %s",  GetLastErrorText(szError, 512) );
		return ;
	}

	time(&t);  
	pTime = asctime(localtime(&t));
	pTime[strlen(pTime)-1] = '\0';
	fprintf( fp, "[%s] %s ", pTime, buf);
	fclose(fp);

}


/*********************************************************************   
**   从系统中取最后一次错误代码，并转换成字符串返回   
*********************************************************************/   
LPTSTR   GetLastErrorText(   LPTSTR   lpszBuf,   DWORD   dwSize   )     
{   
	DWORD   dwRet;   
	LPTSTR   lpszTemp   =   NULL;   

	dwRet   =   FormatMessage(   FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_ARGUMENT_ARRAY,   
		NULL,   
		GetLastError(),   
		LANG_NEUTRAL,   
		(LPTSTR)&lpszTemp,   
		0,   
		NULL   );   

	//   supplied   buffer   is   not   long   enough   
	if(!dwRet||((long)dwSize<(long)dwRet+14))   
		lpszBuf[0]   =   TEXT('\0');   
	else{   
		lpszTemp[lstrlen(lpszTemp)-2]   =   TEXT('\0');     //remove   cr   and   newline   character   
		sprintf(   lpszBuf,   TEXT("%s   (%ld)"),   lpszTemp,   GetLastError()   );   
	}   

	if   (   lpszTemp   )   
		LocalFree((HLOCAL)   lpszTemp   );   

	return   lpszBuf;   
}   

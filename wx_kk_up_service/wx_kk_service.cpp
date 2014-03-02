
#include "wx_kk_service.h"
#include "stdio.h"
#include <tlhelp32.h> 
#include "time.h"

#if _MSC_VER
#define snprintf _snprintf
#endif

//#include "ShareModule.h"
//#pragma comment(lib, "ShareModule.lib")

wx_service * m_pService = 0;

//log
char logFilePath[512]={0};


BOOL wx_service::bInstall;
SERVICE_STATUS_HANDLE wx_service::hServiceStatus;
SERVICE_STATUS wx_service::status;;//SERVICE程序的状态struct   
DWORD wx_service::dwThreadID=0;

SC_HANDLE wx_service::hSCM={0};
SC_HANDLE wx_service::hService={0};

char   wx_service::m_lpServicePathName[512]={0};         //SERVICE程序的EXE文件路径   
char wx_service::szServiceName[256]={0};
BOOL wx_service::bPause=false;
BOOL wx_service::bStop=false;
char wx_service::MonitorExe[128]={"wx_kk_up.exe"};
#define SLEEP_TIME 1000


//服务核心，监控
void  wx_service::Run()
{
	char szError[512]={0};
	
	char path[512]={0};
	char MonitorExePath[512]={0};

	log( "wx_service::Runing ") ;

	GetModuleFileName(0, path, MAX_PATH);
	char *p = strrchr( path, '\\');
	if(p==0)
		return ;
	*p = '\0';
	int index = 0;

	DWORD id = -1;
	PROCESSENTRY32 pInfo = {0}; 
	HANDLE hSnapShot = 0;

	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = {0};

	sprintf(MonitorExePath,"%s\\%s", path, MonitorExe);
	log("MonitorExePath = %s", MonitorExePath);

	while ( true )
	{		
		if(bStop) 
			goto end;

		while(bPause == true){
			if(bStop) goto end;
			Sleep(1000);
		}

		hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0) ;  
	//	log(" ===========         Query Process  ======================== ");
		pInfo.dwSize = sizeof(pInfo); 
		Process32First(hSnapShot, &pInfo) ;
		id = -1;
		do {
		//	log("Process [%s]   ", pInfo.szExeFile);
			if(strcmp(pInfo.szExeFile, MonitorExe) == 0) 
			{ 
				id = pInfo.th32ProcessID ;
				break ;
			} 
		}while( Process32Next(hSnapShot, &pInfo) != FALSE );
		CloseHandle(hSnapShot);

		if (id == -1)
		{
			log(" I think Process [%s] is dump ", MonitorExe);

			si.dwFlags = STARTF_USESHOWWINDOW; 
			si.wShowWindow = SW_SHOW; 

			BOOL bRet = CreateProcess (
				MonitorExePath,
				NULL,
				NULL,
				NULL,
				FALSE,
				DETACHED_PROCESS,
				NULL,
				path,
				&si,
				&pi) ;

			Sleep(SLEEP_TIME);

			if(bRet)
			{
				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
				log(" CreateProcess success. dwProcessId:%d  dw Main ThreadId: %d ",pi.dwProcessId, pi.dwThreadId);
			}
			else
			{
				log(" CreateProcess failure : %s", GetLastErrorText(szError, 512) );
			}

		}
	 	Sleep( SLEEP_TIME );
	}

end:
	return ;
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

wx_service::wx_service()
{
	m_pService = this;
	memset(szServiceName, 0 , 256);
	strcpy(szServiceName, "TF_wx_kk_Service");
}
wx_service::~wx_service()
{

}

void wx_service::Init(){

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
	snprintf( logFilePath, p-pre , "%s", pre);
	strncat(  logFilePath, ("\\wx_kk_up_service.log"), strlen("\\wx_kk_up_service.log"));

	m_lpServicePathName[0] = '\"';
	m_lpServicePathName[strlen(m_lpServicePathName)] = '\"';

	log("Init");
}

BOOL wx_service::Install()
{
	char szError[512]={0};
	bool bRet = false;
	//这里列出主要的两个函数，其它的可以在代码里找。
	//打开服务控制管理器

	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	long ret = GetLastError();
	if(hSCM==NULL){
		LogEvent( GetLastErrorText(szError, 512) );
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
BOOL wx_service::OpenService()
{
	char szError[512]={0};
	bool bRet = false;
	bPause = false;
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
					if(::StartService(hService, NULL, NULL))
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

//暂停服务
BOOL wx_service::PauseService()
{
	bPause = true;
	return true;
}
BOOL wx_service::ResumeService()
{
	bPause = false;
	return true;
}

//停止服务
BOOL wx_service::StopService()
{
	char szError[512]={0};
	bool bRet = false;
	bPause = true;
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



BOOL wx_service::Uninstall()
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


/*********************************************************************   
**   此函数接受系统发来的STOP、PAUSE，并做出相应处理发给Service_Main,   
**   每一个指令执行后需报告当前状态给SCM管理器   
*********************************************************************/   

void   WINAPI   ServiceHandle(DWORD   dwControl)   
{   
	switch(dwControl){   
		  case   SERVICE_CONTROL_STOP:   //要服务停止
			  m_pService->status.dwWaitHint = 5;
			  m_pService->status.dwCurrentState = SERVICE_STOP_PENDING;
			  SetServiceStatus( m_pService->hServiceStatus, &m_pService->status);
			 // m_pService->StopService()  ;  
			  m_pService->bStop = true;
			  Sleep(3000);
			  m_pService->status.dwCurrentState = SERVICE_STOPPED;
			  SetServiceStatus( m_pService->hServiceStatus, &m_pService->status);
			  LogEvent(("Service stopped"));
			  log("Service stopped");
			 // PostThreadMessage( m_pService->dwThreadID,   WM_QUIT,   0,   0   )   ;   
			  break;   
		  case   SERVICE_CONTROL_PAUSE:   //要服务暂停
			  m_pService->status.dwCurrentState = SERVICE_PAUSE_PENDING;
			  SetServiceStatus( m_pService->hServiceStatus, &m_pService->status);
			  m_pService->PauseService();
			  m_pService->status.dwCurrentState = SERVICE_PAUSED;
			  SetServiceStatus( m_pService->hServiceStatus, &m_pService->status);
			  break;   
		  case   SERVICE_CONTROL_CONTINUE:   //要服务继续
			  m_pService->status.dwCurrentState = SERVICE_PAUSE_PENDING;
			  SetServiceStatus( m_pService->hServiceStatus, &m_pService->status);
			  m_pService->ResumeService();
			  m_pService->status.dwCurrentState = SERVICE_RUNNING;
			  SetServiceStatus( m_pService->hServiceStatus, &m_pService->status);
			  break;   
		  case   SERVICE_CONTROL_INTERROGATE:   //要服务马上报告它的状态
			  break;   
		  case SERVICE_CONTROL_SHUTDOWN: //将要关机
			  LogEvent(("将要关机"));
			  log("将要关机");
			  // 1.通知监控的程序进行关闭

			  // 2.停止服务
			  m_pService->bStop = true;
			//  m_pService->StopService();
			  m_pService->status.dwCurrentState = SERVICE_STOPPED;
			  SetServiceStatus( m_pService->hServiceStatus, &m_pService->status);
			  // 3.退出
			  PostThreadMessage( m_pService->dwThreadID,   WM_QUIT,   0,   0   )   ; 
			  break;
		  default:   
			  break;   
	}   
}   



//添加服务主线程函数和控制函数
void WINAPI wx_service::ServiceMain(DWORD   dwArgc,   LPTSTR   *lpszArgv)
{
	log( "ServiceMain : ServeiceName: %s  args: %s ", szServiceName , lpszArgv);
	//注册服务控制
	hServiceStatus = RegisterServiceCtrlHandler( szServiceName, &ServiceHandle);
	if (hServiceStatus == NULL)
	{
		log( "ServiceHandle not installed" );
		LogEvent(("ServiceHandle not installed"));
		return;
	}
	log( "ServiceHandle installed & SetServiceStatus SERVICE_START_PENDING " );
	status.dwWaitHint = 10;
	status.dwCurrentState = SERVICE_START_PENDING;
	status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_SHUTDOWN;//这个要使用，否则你不能控制
	SetServiceStatus(hServiceStatus, &status);

	status.dwWin32ExitCode = S_OK;
	status.dwCheckPoint = 0;
	status.dwWaitHint = 0;
	status.dwCurrentState = SERVICE_RUNNING;
	log( "SetServiceStatus SERVICE_RUNNING" );
	SetServiceStatus( hServiceStatus, &status);

	log( "RUNNING" );
	//运行监控
	dwThreadID   =   GetCurrentThreadId()   ;   
	Run();
	log( "Run() returned" );
	 
	status.dwCurrentState = SERVICE_STOPPED;
	SetServiceStatus(hServiceStatus, &status);
	LogEvent(("Service stopped"));
	 

}

void log(LPCTSTR format, ...)
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
		LogEvent( "fopen Error : ",  GetLastErrorText(szError, 512) );
		return ;
	}

	time(&t);  
	pTime = asctime(localtime(&t));
	pTime[strlen(pTime)-1] = '\0';
	fprintf( fp, "[%s] %s ", pTime, buf);
	fclose(fp);

}

 /*********************************************************************   
  **   将错误信息加进系统EVENT管理器中;   
  *********************************************************************/   
void  LogEvent(LPCTSTR format, ...)
{
	TCHAR         lpszMsg[256];   
	HANDLE       hEventSource;   
	LPCTSTR     lpszStrings[2];   

	char buf[4096]={0}, *p=buf;
	va_list args;
	va_start(args, format);
	p += _vsnprintf(p, sizeof buf - 1, format, args);
	va_end(args);
	// while ( p > buf && isspace(p[-1]) )    *--p = '\0';
	*p = '\0';

	if(!m_pService->hServiceStatus){   
		printf("%s",   buf)   ;   
		return   ;   
	}   
	//   Use   event   logging   to   log   the   error.   

	hEventSource   =   RegisterEventSource( NULL ,   m_pService->szServiceName);   

	sprintf(lpszMsg,   TEXT("\n%s   提示信息:   %d"),   m_pService->szServiceName,   GetLastError());   
	lpszStrings[0]   =   buf;   
	lpszStrings[1]   =   lpszMsg;   

	if(hEventSource   !=   NULL){   

		ReportEvent(hEventSource, //   handle   of   event   source   
			EVENTLOG_ERROR_TYPE, //   event   type   
			0, //   event   category   
			0, //   event   ID   
			NULL, //   current   user's   SID   
			2, //   number   of   strings   in   lpszStrings   
			0, //   no   bytes   of   raw   data   
			lpszStrings, //   array   of   error   strings   
			buf); //   no   raw   data   
		(VOID)   DeregisterEventSource(hEventSource);   
	}   

}
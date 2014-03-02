
// KService.cpp
/**
* Auth: Karl
* Date: 2014/5/8
* LastUpdate:2014/5/8
*/

#include "stdafx.h"

#include "KService.h"


KService::KService()
{

}

KService::~KService()
{

}

long KService::SetService(Service sIn)
{
	this->service = sIn;
	return 0;
}

////注册服务
long KService::Install()
{
	long lRet = 0;

	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(hSCM==NULL){
		return GetLastError();
	}
	SERVICE_DESCRIPTION sd; 
	//创建服务
	hService = ::CreateService(
		hSCM, 
		this->service.szServiceName,//服务名称
		this->service.szServiceDisplayName,//显示名称
		SERVICE_ALL_ACCESS, SERVICE_WIN32_SHARE_PROCESS,
		SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
		service.szServicePath, //服务程序路径
		NULL, NULL, (""), NULL, NULL);
	if(hService){
		lRet = 0;
		sd.lpDescription = this->service.szServiceDescription;
		if( !ChangeServiceConfig2( 
			hService,                 // handle to service  
			SERVICE_CONFIG_DESCRIPTION, // change: description  
			&sd) )                      // new description  
		{ 
			//printf("ChangeServiceConfig2 failed\n"); 
			lRet = GetLastError();
		} 
	}else{
		lRet = GetLastError();
		CloseServiceHandle(hSCM);
		return lRet;
	}
	
end:
	::CloseServiceHandle(hService);
	::CloseServiceHandle(hSCM);

	return lRet;

}

//打开 [服务管理器] 和 [服务] 句柄
//返回值:\
	#define ERROR	-1	//错误\
	#define NONE	-2	//空\
//
long OpenServiceHandle(SC_HANDLE &hSCM, SC_HANDLE &hService, char* serviceName)
{
	//打开 [服务管理器] 和 [服务] 句柄
	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(hSCM==NULL){
		return GetLastError();
	}
	hService = ::OpenService(         //获取SERVICE控制句柄的API   
		hSCM,                         //SCM管理器句柄   
		serviceName,                 //SERVICE内部名称，控制名称   
		SERVICE_ALL_ACCESS);         //打开的权限，删除就要全部权限
	if(hService==NULL){
		::CloseServiceHandle(hSCM);
		return GetLastError();
	}
	return 0;
}



//查询服务是否存在
//返回值:\
	#define ERROR	-1	//错误\
	#define NONE	0	//空\
	#define OK		1	//存在，正常
//
long KService::ServiceExist(char *serviceName)
{
	long lRet = OpenServiceHandle(hSCM, hService,serviceName);
	::CloseServiceHandle( hService);
	::CloseServiceHandle( hSCM);
	return lRet;
}

//获取服务信息
long KService::QueryService(char *serviceName, Service *pService)
{
	if(pService==0){
	//	assert(pService);
		return GetLastError();
	}
	long lRet = OpenServiceHandle(hSCM, hService, serviceName);
	if( lRet != 0)
	{
		return  lRet;
	}

	DWORD dwSize=1024;
	char* szInfo[1024]={0};

	sprintf(pService->szServiceName, "%s", serviceName);//服务名称
	if( !QueryServiceStatus(hService,   &pService->status)){//查询服务状态
		lRet =  GetLastError();
		goto end;
	}

	dwSize = 1024;
	QueryServiceConfig2( hService, SERVICE_CONFIG_DESCRIPTION,(LPBYTE)szInfo,dwSize,&dwSize);
	if( ((LPSERVICE_DESCRIPTION)szInfo)->lpDescription!=0 )
		sprintf( pService->szServiceDescription, "%s", ((LPSERVICE_DESCRIPTION)szInfo)->lpDescription);//得到服务描述信息
	
	dwSize = 1024;
	QueryServiceConfig( hService,(LPQUERY_SERVICE_CONFIG)szInfo, dwSize, &dwSize);
	sprintf(pService->szServiceDisplayName,  "%s", ((LPQUERY_SERVICE_CONFIG)szInfo)->lpDisplayName);//服务显示名称
	sprintf(pService->szServicePath,  "%s", ((LPQUERY_SERVICE_CONFIG)szInfo)->lpBinaryPathName);//服务程序路径
	pService->status.dwServiceType = ((LPQUERY_SERVICE_CONFIG)szInfo)->dwServiceType;//服务类型
	sprintf(pService->szUser,  "%s", ((LPQUERY_SERVICE_CONFIG)szInfo)->lpServiceStartName);//服务启动用户

end:
	::CloseServiceHandle( hService);
	::CloseServiceHandle( hSCM);

	return 0;
}

Service** KService::GetServices()
{
	listService.clear();//clear

	LPENUM_SERVICE_STATUS lst=NULL;//注意这个lst
	DWORD ret=0;
	DWORD size=0;
	
	SC_HANDLE sc = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	SC_HANDLE sh;
	char* szInfo[1024*8];
	DWORD dwSize=1024*8;
	CString str;
	//第一次调用来得到需要多大的内存区
	EnumServicesStatus(sc,SERVICE_WIN32,SERVICE_STATE_ALL,lst,size,&size,&ret,NULL);
	//申请需要的内存
	lst=(LPENUM_SERVICE_STATUS)LocalAlloc(LPTR,size);
	EnumServicesStatus(sc,SERVICE_WIN32,SERVICE_STATE_ALL,lst,size,&size,&ret,NULL);
	//开始记录枚举出服务的信息
	for(DWORD i=0;i<ret;i++){
		dwSize=1024*8;
		ZeroMemory(szInfo,dwSize);
		Service *info = new Service(); 
		sprintf( info->szServiceName, "%s", lst[i].lpServiceName);
		sprintf( info->szServiceDisplayName, "%s", lst[i].lpDisplayName);
		info->status.dwCurrentState = lst[i].ServiceStatus.dwCurrentState;
		sh=OpenService(sc, lst[i].lpServiceName,SERVICE_ALL_ACCESS);
		//得到服务描述信息
		QueryServiceConfig2(sh,SERVICE_CONFIG_DESCRIPTION,(LPBYTE)szInfo,dwSize,&dwSize);
		sprintf( info->szServiceDescription, "%s", ((LPSERVICE_DESCRIPTION)szInfo)->lpDescription);
		//得到服务的启动账户名
		ZeroMemory(szInfo,dwSize);
		dwSize=1024*8;
		QueryServiceConfig(sh,(LPQUERY_SERVICE_CONFIG)szInfo,dwSize,&dwSize);
		sprintf( info->szUser, "%s", ((LPQUERY_SERVICE_CONFIG)szInfo)->lpServiceStartName);
		CloseServiceHandle(sh);
		//添加到信息队列中
	//	ItemAdd(&info);
	}
	CloseServiceHandle(sc);

	return NULL;
}


//卸载服务
long KService::UnInstall()
{
	long lRet = OpenServiceHandle(hSCM, hService, service.szServiceName);
	if( lRet != 0)
	{
		return  lRet;
	}
	if( !QueryServiceStatus(hService,   &status)){
		lRet =  GetLastError();
		goto end;
	}
	if( (status.dwCurrentState == SERVICE_RUNNING) )
	{
		if(  ControlService(hService,   SERVICE_CONTROL_STOP, &status) ){
			//直接向SERVICE发STOP命令，如果能够执行到这里，说明SERVICE正运行   
			//那就需要停止程序执行后才能删除   
			Sleep(2000)   ;   //等3秒使系统有时间执行STOP命令  
			int times=0;
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
		}else{
			goto end;
		}
	}
	if(status.dwCurrentState   !=   SERVICE_STOPPED)  
	{          
		//如果SERVICE接受STOP命令后还没有STOPPED   
		lRet =  GetLastError();
		goto end;
	}else{
		//删除指令在这里   
		if( ::DeleteService(hService)){
			lRet = 0;
		}else{
			lRet = GetLastError();
			goto end;
		}
	} 

end:
	::CloseServiceHandle( hService);
	::CloseServiceHandle( hSCM);

	return lRet;
}

// 启动服务
long KService::Start()
{
	long lRet = 0;
	
	if( OpenServiceHandle(hSCM, hService, service.szServiceName) != 0 ){
		return  GetLastError();
	}
	if( !QueryServiceStatus(hService,   &status)){
		goto end;
	}
	int  times = 0;
	// 启动服务
	if( status.dwCurrentState == SERVICE_STOPPED)
	{
		if( ::StartService( hService, NULL, NULL) == FALSE)
		{
			lRet =  GetLastError();
			goto end;
		}
		// 等待服务启动
		while( ::QueryServiceStatus( hService, &status) == TRUE)
		{
			times ++;
			::Sleep( status.dwWaitHint);
			if( status.dwCurrentState == SERVICE_RUNNING)
			{
				lRet = 0;//服务启动成功
				goto end;
			}
			if(times>10){
				lRet =  GetLastError();
				break;
			}
		}
	}

end:
	::CloseServiceHandle( hService);
	::CloseServiceHandle( hSCM);
	return lRet;
}

//停止服务
long KService::Stop()
{
	long lRet = 0;

	if( OpenServiceHandle(hSCM, hService, service.szServiceName) != 0 ){
		return  GetLastError();
	}
	if( !QueryServiceStatus(hService,   &status)){
		lRet =  GetLastError();
		goto end;
	}

	if( ControlService(hService,   SERVICE_CONTROL_STOP, &status) ){
		//直接向SERVICE发STOP命令，如果能够执行到这里，说明SERVICE正运行   
		//那就需要停止程序执行后才能删除   
		Sleep(2000)   ;   //等3秒使系统有时间执行STOP命令  
		int times=0;
		while( QueryServiceStatus(hService,   &status)   )   {
			times ++;
			if(status.dwCurrentState   ==   SERVICE_STOP_PENDING)   
			{         //如果SERVICE还正在执行(PENDING)停止任务   
				Sleep(500)   ;         //那就等1秒钟后再检查SERVICE是否停止OK  
			}else {
				break ; //STOP命令处理完毕，跳出循环   
			}
			if(times>10){
				break;// time out 
			}
		}//循环检查SERVICE状态结束   
	}else{
		lRet =  GetLastError();
		goto end;
	}
	if(status.dwCurrentState   !=   SERVICE_STOPPED)  
	{          //如果SERVICE接受STOP命令后还没有STOPPED   
		lRet =  GetLastError();
		goto end;
	}else{
		lRet = 0;
	} 

end:
	::CloseServiceHandle( hService);
	::CloseServiceHandle( hSCM);
	return lRet;
}
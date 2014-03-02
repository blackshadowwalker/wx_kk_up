
// main.cpp
#pragma once

#include "stdio.h"


#include "wx_kk_service.h"

int main(int argc, char **argv)
{
	SetConsoleTitle("无线测试服务程序");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED); 
	printf("%s \n",  argv[0] );

	char lpCmdLine[64] = {0};
	char szServiceName[256] = {"TF_wx_kk_Service"};

	if(argc>1)
		sprintf(lpCmdLine, argv[1]);

	wx_service sv ;
	sv.Init();

	long dwThreadID = ::GetCurrentThreadId();
	
	if (stricmp(lpCmdLine, "-install") == 0)
	{
		if(argc>=3)
		strcpy( szServiceName, argv[2]);
		strcpy( sv.szServiceName, szServiceName);
		if(sv.Install())
			printf("Install Success.");
		else
			printf("Install Failed.");
		Sleep(2000);
	}
	else if (stricmp(lpCmdLine, "-uninstall") == 0)
	{
		if(argc>=3)
		strcpy( szServiceName, argv[2]);
		strcpy( sv.szServiceName, szServiceName);
		if(sv.Uninstall())
			printf("unInstall Success.");
		else
			printf("unInstall Failed.");
		Sleep(2000);
	}
	else if (stricmp(lpCmdLine, "-help") == 0 || stricmp(lpCmdLine, "/?") == 0)
	{
		printf(" Help: \n");
		printf(" wx_kk_service.exe [options argv]  \n");
		printf(" e.g:  \n");
		printf(" wx_kk_service.exe -install serverName  \n");
		printf(" wx_kk_service.exe -uninstall serverName  \n");
	}
	else //if( stricmp(lpCmdLine, "-service") )
	{
		log(" StartServiceCtrlDispatcher ... argc=%d", argc); 
		SERVICE_TABLE_ENTRY st[] =
		{
			{ szServiceName, (LPSERVICE_MAIN_FUNCTION)sv.ServiceMain },
			{ NULL, NULL }
		};
		if(argc>=2){
			strcpy( st[0].lpServiceName, argv[1]);
			strcpy( sv.szServiceName, argv[1]);
			
			log(" arg2 = %s", argv[1]);
		}
		log(" ServiceName : %s ", st[0].lpServiceName); 

		char error[512]={0};
		if (!::StartServiceCtrlDispatcher(st))
		{
			log("Register Service[%s] Main Function Error! %s", st[0].lpServiceName, GetLastErrorText(error, 512) );
		}
	}

//	getchar();
END:
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),7); 
	
	return 0;
}


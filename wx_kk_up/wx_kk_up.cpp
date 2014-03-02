
// wx_kk_up.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "wx_kk_up.h"
#include "wx_kk_upDlg.h"

#include "TCode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Cwx_kk_upApp

BEGIN_MESSAGE_MAP(Cwx_kk_upApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// Cwx_kk_upApp 构造

Cwx_kk_upApp::Cwx_kk_upApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 Cwx_kk_upApp 对象

Cwx_kk_upApp theApp;


// Cwx_kk_upApp 初始化

BOOL Cwx_kk_upApp::InitInstance()
{
	FileUtil::CreateFolders("zlog");
	dzlog_init("logcfg.conf", "my_cat");

	dzlog_info("start this program!");

	CCrashHandler ch;//后面用来捕获一个异常
	ch.SetProcessExceptionHandlers();//设置一个对整个程序有效的异常处理函数（如SEH)
	ch.SetThreadExceptionHandlers();//设置仅对当前个线程有效的异常处理函数（如预料外异常或终止处理函数）
	
	dzlog_info("started carshHandler ");



	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));


		
	//获取控件注册状态
	char temp[256]={0};
	HRESULT hr = -1;
	CLSID clsid = {0};
	char pClsid[256]={0};
	LONG lValue = 256;
	LPWSTR lpClsid;
	hr = CLSIDFromProgID(  L"HTAgent.HTAgentObj",&clsid);
	if(hr==S_OK){
		if( StringFromCLSID( clsid, &lpClsid) == S_OK)
		{
			wcharTochar( lpClsid, pClsid);
			//GetDllPath
			HKEY hKey;     
			BOOL bPresent;     
			TCHAR szPath[MAX_PATH];     
			DWORD dwRegType;     
			DWORD cbData   =   sizeof   szPath   *   sizeof   TCHAR;    
			sprintf(temp, "CLSID\\%s\\InprocServer32",pClsid); 
			if(RegOpenKeyEx(HKEY_CLASSES_ROOT, temp, 0, KEY_READ,&hKey)==ERROR_SUCCESS){
				if( RegQueryValue(hKey,NULL, temp, &lValue ) == ERROR_SUCCESS){
				//	m_dllPath.Format("%s",temp);
				}
			}
		}
	}else{
		MessageBox(0,"控件未注册，请先注册控件","系统提示", MB_OK | MB_ICONHAND );
		WinExec("InstanceConfig.exe",SW_SHOW);
		::PostQuitMessage(0);
	}


	Cwx_kk_upDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

int Cwx_kk_upApp::ExitInstance()
{
	zlog_fini();

	return CWinAppEx::ExitInstance();
}

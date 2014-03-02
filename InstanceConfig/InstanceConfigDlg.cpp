
// InstanceConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "InstanceConfig.h"
#include "InstanceConfigDlg.h"

#include "FileUtil.h"
#include "TCode.h"

#include "KService.h"
 
char szWatchedExeName[64] = {"wx_kk_up.exe"};//被监控的程序名称
char szServiceExeName[64] = {"wx_kk_up_service.exe"};//监控服务程序命名
char szServiceName[64] = {"TF_wx_kk_Service"};//监控服务名称
char m_lpServicePathName[512]={0};//监控服务程序路径

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CInstanceConfigDlg::CInstanceConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInstanceConfigDlg::IDD, pParent)
	, m_dllPath(_T(""))
	, m_serviceName(_T(""))
	, m_serviceDisplayName(_T(""))
	, m_serviceDescription(_T(""))
	, m_servicePath(_T(""))
	, m_serviceParam(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CInstanceConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_DLL_PATH, m_dllPath);
	DDX_Text(pDX, IDC_SERVICE_NAME, m_serviceName);
	DDX_Text(pDX, IDC_SERVICE_DISPLAY_NAME, m_serviceDisplayName);
	DDX_Text(pDX, IDC_SERVICE_DESCRIPTION, m_serviceDescription);
	DDX_Control(pDX, IDC_SERVICE_STRAT_TYPE, m_serviceStartType);
	DDX_Text(pDX, IDC_SERVICE_PATH, m_servicePath);
	DDX_Control(pDX, IDC_SERVICE_SELECT, m_serviceType);
	DDX_Text(pDX, IDC_SERVICE_PARAM, m_serviceParam);
}

BEGIN_MESSAGE_MAP(CInstanceConfigDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BT_INSTALL_SERVICE, &CInstanceConfigDlg::OnBnClickedBtInstallService)
	ON_BN_CLICKED(IDC_BT_UNISTALL_SERVICE, &CInstanceConfigDlg::OnBnClickedBtUnistallService)
	ON_BN_CLICKED(IDC_BT_START_SERVICE, &CInstanceConfigDlg::OnBnClickedBtStartService)
	ON_BN_CLICKED(IDC_BT_STOP_SERVICE, &CInstanceConfigDlg::OnBnClickedBtStopService)
	ON_BN_CLICKED(IDC_BT_CLOSE_PROCESS, &CInstanceConfigDlg::OnBnClickedBtCloseProcess)
	ON_BN_CLICKED(IDC_BT_BROWSER_DLL, &CInstanceConfigDlg::OnBnClickedBtBrowserDll)
	ON_BN_CLICKED(IDC_BT_DLL_REG, &CInstanceConfigDlg::OnBnClickedBtDllReg)
	ON_BN_CLICKED(IDC_BT_DLL_UNREG, &CInstanceConfigDlg::OnBnClickedBtDllUnreg)
	ON_BN_CLICKED(IDC_BT_BROWSER_SERVER_EXE, &CInstanceConfigDlg::OnBnClickedBtBrowserServerExe)
	ON_CBN_SELCHANGE(IDC_SERVICE_SELECT, &CInstanceConfigDlg::OnCbnSelchangeServiceSelect)
	ON_BN_CLICKED(IDC_BT_INSTALL_HTTP, &CInstanceConfigDlg::OnBnClickedBtInstallHttp)
	ON_BN_CLICKED(IDC_BT_HTTP_CONFIG, &CInstanceConfigDlg::OnBnClickedBtHttpConfig)
END_MESSAGE_MAP()


BOOL CInstanceConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	
	memset(thisPath, 0, MAX_PATH);
	//获取当前路径  ===============================
	::GetModuleFileName( 0, thisPath, 512);
	char *p = strrchr(thisPath, '\\');
	if(p)
		*p='\0';
	// end of 获取当前路径 ========================


	int i=0;

	//服务类型
	int size = sizeof(ServiceTypeName)/sizeof(ServiceTypeName[0]);
	for( i=0; i < size; i++)
		m_serviceType.AddString( ServiceTypeName[i]);
//	m_serviceType.SetCurSel(0);

	//服务启动类型
	int sizeOfStartType = sizeof(StartType)/sizeof(StartType[0]);
	for( i=0; i < sizeOfStartType; i++)
		m_serviceStartType.AddString( StartType[i]);
	m_serviceStartType.SetCurSel(2);

	//服务程序路径
	//m_servicePath.Format("%s\\wx_kk_up_service.exe", thisPath);

	//获取控件注册状态
	char temp[256]={0};
	HRESULT hr = -1;
	CLSID clsid = {0};
	char pClsid[256]={0};
	LONG lValue = 256;
	LPWSTR lpClsid;
	hr = CLSIDFromProgID(  L"HTAgent.HTAgentObj",&clsid);
	if(hr==S_OK){
		GetDlgItem(IDC_BT_DLL_REG)->EnableWindow(false);
		GetDlgItem(IDC_BT_DLL_UNREG)->EnableWindow(true);
		if( StringFromCLSID( clsid, &lpClsid) == S_OK)
		{
			wcharTochar( lpClsid, pClsid);
			//GetDllPath
			HKEY hKey;     
			BOOL bPresent;     
			TCHAR szPath[_MAX_PATH];     
			DWORD dwRegType;     
			DWORD cbData   =   sizeof   szPath   *   sizeof   TCHAR;    
			sprintf(temp, "CLSID\\%s\\InprocServer32",pClsid); 
			if(RegOpenKeyEx(HKEY_CLASSES_ROOT, temp, 0, KEY_READ,&hKey)==ERROR_SUCCESS){
				if( RegQueryValue(hKey,NULL, temp, &lValue ) == ERROR_SUCCESS){
					m_dllPath.Format("%s",temp);
					UpdateData(false);
				}
			}
		}
	}else{
		GetDlgItem(IDC_BT_DLL_REG)->EnableWindow(true);
		GetDlgItem(IDC_BT_DLL_UNREG)->EnableWindow(false);
	}

	UpdateStatus();

	return TRUE;  
}


void CInstanceConfigDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CInstanceConfigDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CInstanceConfigDlg::OnBnClickedBtBrowserServerExe()
{
	//浏览控件EXE
	UpdateData(true);
	char path[512] = {0};
	GetModuleFileName( NULL, path, 512);
	if( !m_dllPath.Trim().IsEmpty() )
		sprintf(path, "%s", m_dllPath.GetBuffer(m_dllPath.GetLength()) );
	CFileDialog fDlg(TRUE, "服务程序(*.exe)|*.exe | 服务程序(*.dll;*.ocx;*.sys)|*.dll;*.ocx;*.sys|All Files (*.*)|*.*||",  path, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT );
	if( fDlg.DoModal() == IDOK )
	{
		if( !m_serviceName.IsEmpty()){
			m_servicePath.Format("\"%s\" %s", fDlg.GetPathName(), m_serviceParam);
		}else{
			MessageBox("请先填写服务名称");
		}
	}
	UpdateData(false);
}


bool CInstanceConfigDlg::UpdateStatus()
{
	GetDlgItem(IDC_BT_INSTALL_SERVICE)->EnableWindow(false);
	GetDlgItem(IDC_BT_UNISTALL_SERVICE)->EnableWindow(false);
	GetDlgItem(IDC_BT_START_SERVICE)->EnableWindow(false);
	GetDlgItem(IDC_BT_STOP_SERVICE)->EnableWindow(false);

	char szError[512]={0};
	bool bRet = false;
	long lRet = 0;
	KService ksv;
	
	if(m_serviceName.IsEmpty())
		goto end;

	m_serviceDisplayName = "";
	m_serviceDescription = "";
	m_servicePath = "";
	m_serviceParam = "";

	//查询服务是否已经安装
	lRet = ksv.ServiceExist( m_serviceName.GetBuffer(m_serviceName.GetLength()) );
	if( lRet == 0 )
	{
		//已经安装
		GetDlgItem(IDC_BT_UNISTALL_SERVICE)->EnableWindow(true);

		//设置按钮和服务状态信息
		Service sv;
		lRet = ksv.QueryService( m_serviceName.GetBuffer(m_serviceName.GetLength()), &sv);
		if(lRet==0)
		{
			m_serviceName.Format("%s", sv.szServiceName);
			m_serviceDisplayName.Format("%s", sv.szServiceDisplayName);
			m_serviceDescription.Format("%s", sv.szServiceDescription);
			m_servicePath.Format("%s", sv.szServicePath);
			char *p = strchr( sv.szServicePath, ' ');
			if(p)
				m_serviceParam.Format("%s", p);

			if( sv.status.dwCurrentState == SERVICE_RUNNING){
				GetDlgItem(IDC_BT_START_SERVICE)->EnableWindow(false);
				GetDlgItem(IDC_BT_STOP_SERVICE)->EnableWindow(true);
				GetDlgItem(IDC_STATUS_SERVICE)->SetWindowText("正在运行...");
			}else if( sv.status.dwCurrentState == SERVICE_STOPPED){
				GetDlgItem(IDC_BT_START_SERVICE)->EnableWindow(true);
				GetDlgItem(IDC_BT_STOP_SERVICE)->EnableWindow(false);
				GetDlgItem(IDC_STATUS_SERVICE)->SetWindowText("已停止");
			}else{
				GetDlgItem(IDC_BT_START_SERVICE)->EnableWindow(false);
				GetDlgItem(IDC_BT_STOP_SERVICE)->EnableWindow(false);
				GetDlgItem(IDC_STATUS_SERVICE)->SetWindowText("状态不正常!");
			}
		}else{
			MessageBox( GetLastErrorText( lRet, szError, 512) , 0, MB_ICONHAND  );
		}
		
	}else if( lRet == 1060 ){
		 //服务不存在
		GetDlgItem(IDC_BT_INSTALL_SERVICE)->EnableWindow(true);
	}else{
		//错误
		MessageBox( GetLastErrorText( lRet, szError, 512) , 0, MB_ICONHAND  );
		goto end;
	}

end:
	return lRet;
}

//注册服务
void CInstanceConfigDlg::OnBnClickedBtInstallService()
{
	UpdateData(true);
	char szError[512]={0};
	long lRet = 0;

	KService ksv;
	Service sv;

	if(m_serviceName.IsEmpty()){
		MessageBox("请填写服务名称");
		return;
	}
	if(m_serviceDisplayName.IsEmpty()){
		MessageBox("请填写服务显示名称");
		return;
	}
	if(m_servicePath.IsEmpty()){
		MessageBox("请选择服务程序");
		return;
	}
	sprintf(sv.szServiceName, "%s", m_serviceName);
	sprintf(sv.szServiceDisplayName,"%s", m_serviceDisplayName);
	sprintf(sv.szServicePath, "%s", m_servicePath);
	sprintf(sv.szServiceDescription, "%s", m_serviceDescription);
	ksv.SetService(sv);
	lRet = ksv.Install();
	if( lRet==0 )
	{
		GetDlgItem(IDC_BT_UNISTALL_SERVICE)->EnableWindow(true);
		GetDlgItem(IDC_BT_START_SERVICE)->EnableWindow(true);
		GetDlgItem(IDC_BT_STOP_SERVICE)->EnableWindow(false);
		int ret = MessageBox("服务安装成功, 是否现在打开服务?", NULL, MB_OKCANCEL ) ;
		if( ret == IDOK ){
			OnBnClickedBtStartService();
		}
	}else{
		MessageBox( GetLastErrorText(lRet, szError, 512) , 0, MB_ICONHAND  );
	}
}

//卸载服务
void CInstanceConfigDlg::OnBnClickedBtUnistallService()
{
	UpdateData(true);
	char szError[512]={0};
	long lRet = 0;
	//卸载服务
	KService ksv;
	Service sv;
	sprintf(sv.szServiceName, "%s", m_serviceName);
	lRet = ksv.SetService(sv);
	GetDlgItem(IDC_STATUS_SERVICE)->SetWindowText("正在卸载...");
	lRet = ksv.UnInstall();
	if( lRet==0 ){
		GetDlgItem(IDC_BT_INSTALL_SERVICE)->EnableWindow(true);
		GetDlgItem(IDC_BT_UNISTALL_SERVICE)->EnableWindow(false);
		GetDlgItem(IDC_BT_START_SERVICE)->EnableWindow(false);
		GetDlgItem(IDC_BT_STOP_SERVICE)->EnableWindow(false);
		GetDlgItem(IDC_STATUS_SERVICE)->SetWindowText("服务已卸载!");
		MessageBox("卸载成功");
	}else{
		GetDlgItem(IDC_STATUS_SERVICE)->SetWindowText("...");
		MessageBox( GetLastErrorText(lRet,szError, 512) , 0, MB_ICONHAND  );
	}
}

// 启动服务
void CInstanceConfigDlg::OnBnClickedBtStartService()
{
	UpdateData(true);
	char szError[512]={0};
	long lRet = 0;
	// 启动服务

	KService ksv;
	Service sv;
	sprintf(sv.szServiceName, "%s", m_serviceName);
	ksv.SetService(sv);
	GetDlgItem(IDC_STATUS_SERVICE)->SetWindowText("正在开启...");
	lRet = ksv.Start();
	if( lRet==0 )
	{
		GetDlgItem(IDC_BT_START_SERVICE)->EnableWindow(false);
		GetDlgItem(IDC_BT_STOP_SERVICE)->EnableWindow(true);
		GetDlgItem(IDC_STATUS_SERVICE)->SetWindowText("服务已开启");
	}else{
		GetDlgItem(IDC_STATUS_SERVICE)->SetWindowText("...");
		MessageBox( GetLastErrorText( lRet, szError, 512) , 0, MB_ICONHAND  );
	}

}

void CInstanceConfigDlg::OnBnClickedBtStopService()
{
	UpdateData(true);
	//停止服务
	char szError[512]={0};
	long lRet = 0;

	KService ksv;
	Service sv;
	sprintf(sv.szServiceName, "%s", m_serviceName);
	ksv.SetService(sv);
	GetDlgItem(IDC_STATUS_SERVICE)->SetWindowText("正在停止...");
	lRet = ksv.Stop();
	if( lRet==0 )
	{
		GetDlgItem(IDC_BT_START_SERVICE)->EnableWindow(true);
		GetDlgItem(IDC_BT_STOP_SERVICE)->EnableWindow(false);
		GetDlgItem(IDC_STATUS_SERVICE)->SetWindowText("服务已停止");
	}else{
		GetDlgItem(IDC_STATUS_SERVICE)->SetWindowText("...");
		MessageBox( GetLastErrorText( lRet, szError, 512) , 0, MB_ICONHAND  );
	}
}

void CInstanceConfigDlg::OnBnClickedBtCloseProcess()
{
	//关闭后台运行的上传程序

	char cmd[512] = {0};
	sprintf(cmd, "taskkill /f /im  %s ",szWatchedExeName); 
	system(cmd);

}

//判断是否注册  
BOOL IsRegistedDll(char *CLSID)
{
	HKEY hKey;     
	BOOL bPresent;     
	TCHAR szPath[_MAX_PATH];     
	DWORD dwRegType;     
	DWORD cbData   =   sizeof   szPath   *   sizeof   TCHAR;    

	if(RegOpenKeyEx(HKEY_CLASSES_ROOT,"ActiveX.ActiveXControl//Clsid",0,KEY_READ,&hKey)!=ERROR_SUCCESS)
		return false;
	else
		return true;
	//ActiveX.ActiveXControl为控件的名称和内部名称   如"NTGRAPH.NTGraphCtrl.1//CLSID"

}

void CInstanceConfigDlg::OnBnClickedBtBrowserDll()
{
	//浏览控件
	UpdateData(true);
	char path[512] = {0};
	GetModuleFileName( NULL, path, 512);
	if( !m_dllPath.Trim().IsEmpty() )
		sprintf(path, "%s", m_dllPath.GetBuffer(m_dllPath.GetLength()) );
	CFileDialog fDlg(TRUE, "控件(*.dll;*.ocx;*.sys)|*.dll;*.ocx;*.sys|All Files (*.*)|*.*||",  path, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT );
	if( fDlg.DoModal() == IDOK )
	{
		m_dllPath.Format("%s", fDlg.GetPathName());
	}
	UpdateData(false);
}

void CInstanceConfigDlg::OnBnClickedBtDllReg()
{
	// 注册控件
	UpdateData(true);
	bool bRet = false;
	HINSTANCE hLib = LoadLibrary( m_dllPath);      //指定要注册的ocx文件的路径及文件名。

	if (hLib == NULL)
	{
		MessageBox("不能载入控件文件!");
		return;
	}

	//获取注册函数DllRegisterServer地址
	FARPROC lpDllEntryPoint;
	lpDllEntryPoint = GetProcAddress(hLib,"DllRegisterServer");

	//调用注册函数DllRegisterServer
	if(lpDllEntryPoint!=NULL)
	{
		if(FAILED((*lpDllEntryPoint)()))
		{
			//MessageBox(Handle,"调用DllRegisterServer失败!","失败ing..",MB_OK);
			bRet = false;
			MessageBox("控件注册失败!");
			FreeLibrary(hLib);
			goto end;
		};
		//MessageBox(Handle,"注册成功!","Reg",MB_OK);
		bRet = true;
		MessageBox("控件注册成功!");
	}
	else{
		bRet = false;
		MessageBox("控件注册失败!");
		//MessageBox(Handle,"调用DllRegisterServer失败!","失败ing..",MB_OK);
	}
end:
	GetDlgItem(IDC_BT_DLL_REG)->EnableWindow( !bRet);
	GetDlgItem(IDC_BT_DLL_UNREG)->EnableWindow(bRet);
}

void CInstanceConfigDlg::OnBnClickedBtDllUnreg()
{
	UpdateData(true);
	bool bRet = false;
	// 卸载控件
	HINSTANCE hLib = LoadLibrary( m_dllPath );          //指定要注册的ocx文件的路径及文件名。
	if (hLib==NULL)
	{
		//MessageBox(Handle,"不能载入Dll文件!","失败ing..",MB_OK);
		MessageBox("不能载入控件文件!");
		return;
	}
	//获取注册函数DllRegisterServer地址
	FARPROC lpDllEntryPoint;
	lpDllEntryPoint = GetProcAddress(hLib,"DllUnregisterServer");//注意这里是DllUnregisterServer。

	//调用注册函数DllRegisterServer
	if(lpDllEntryPoint!=NULL)
	{
		if(((*lpDllEntryPoint)()))
		{
			bRet = false;
			//MessageBox(Handle,"调用DllUnRegisterServer失败！","失败ing..",MB_OK);
			MessageBox("调用控件DllUnRegisterServer失败!!!");
			FreeLibrary(hLib);
			goto end;
		};
		bRet = true;
		//MessageBox(Handle,"注销成功！","Unreg",MB_OK);
		MessageBox("注销控件成功！");
	}
	else{
		bRet = false;
		//MessageBox(Handle,"调用DllUnRegisterServer失败!!!","结果",MB_OK);
		MessageBox("调用控件DllUnRegisterServer失败!!!");
		goto end;
	}
end:
	GetDlgItem(IDC_BT_DLL_REG)->EnableWindow(bRet);
	GetDlgItem(IDC_BT_DLL_UNREG)->EnableWindow(!bRet);
}


void CInstanceConfigDlg::OnCbnSelchangeServiceSelect()
{
	//选择服务
	int len = sizeof(ServiceTypeName) / sizeof(ServiceTypeName[0]);
	int index = m_serviceType.GetCurSel();
	if(index >=0 && index<len){
		m_serviceName.Format("%s", ServiceName[index]);
		GetDlgItem(IDC_SERVICE_SELECT)->EnableWindow(false);
		//m_serviceType.EnableWindow(false);
		UpdateStatus();
		m_serviceType.EnableWindow(true);
		//MessageBox(ServiceTypeName[index]);
		UpdateData(false);
	}
}

void CInstanceConfigDlg::OnBnClickedBtInstallHttp()
{
	//HTTP服务安装
	//Install Http
	char cmd[512]={0};
	sprintf(cmd, "\"%s\\httpd\\bin\\Apache.exe\" -k install -n %s & pause", thisPath, m_serviceName);
	system(cmd);
}

void CInstanceConfigDlg::OnBnClickedBtHttpConfig()
{
	//HTTP服务配置
	char cmd[512]={0};
	sprintf(cmd, "notepad.exe  \"%s\\httpd\\conf\\httpd.conf\"", thisPath);
	WinExec(cmd, SW_SHOW);
}

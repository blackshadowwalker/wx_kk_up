
// wx_kk_upDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "wx_kk_up.h"
#include "wx_kk_upDlg.h"

#include "stdio.h"
#include  <io.h>
#include "time.h"

//#include "ShareModule.h"

#include "minIni.h"

#include "wxCore.h"

#include "ProcessState.h"
#include "HttpClient.h"
#include "TCode.h"



#if _MSC_VER
#define snprintf _snprintf
#endif
 

//特征图片后缀标志(号牌图片)
#define PLATE_END  "_plate.jpg"

// === static var =========================================================

SystemConfig	Cwx_kk_upDlg::systemConfig = {0};//系统环境设置
KKConfig		Cwx_kk_upDlg::kkConfig = {0};//卡口配置

// ==================================================================

TCHAR szTemp[TF_MAX_PATH_LEN]={0};//temp error info for zlog|tempvar

int	m_ExitCode = 0;//for thread if exit thread
bool threadRuning = false;

#define MAX_ERROR  1353
char LastError [MAX_ERROR][128] = {0};

char LastErrorInfo[512]={0};
char* GetLastErrorInfo()
{
	LPVOID lpMsgBuf;
	char *p = LastErrorInfo;
	long lError = GetLastError();
	if( lError < MAX_ERROR)
		sprintf( LastErrorInfo, "(%d, 0x%x)[%s]", lError, lError, LastError[lError]);
	else{
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			lError,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf,
			0, NULL );
		sprintf( LastErrorInfo, "(%d, 0x%x)[%s]", lError, lError,  lpMsgBuf);
	}
	return p;
}



//===============================================================================

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClickedStaticHttpTeleframeCn();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::OnStnClickedStaticHttpTeleframeCn()
{
	ShellExecute(NULL, NULL, _T("http://teleframe.cn"), NULL, NULL, SW_SHOWNORMAL);
}


void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_STN_CLICKED(IDC_STATIC_HTTP_TELEFRAME_CN, &CAboutDlg::OnStnClickedStaticHttpTeleframeCn)
END_MESSAGE_MAP()


// Cwx_kk_upDlg 对话框




Cwx_kk_upDlg::Cwx_kk_upDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Cwx_kk_upDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

}
Cwx_kk_upDlg::~Cwx_kk_upDlg()
{
 
}

void Cwx_kk_upDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HTAGENTOBJ1, m_agent);
	DDX_Control(pDX, IDC_LIST1, m_LogList);
}

BEGIN_MESSAGE_MAP(Cwx_kk_upDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_COMMAND(ABOUTBOX, &Cwx_kk_upDlg::OnAboutbox)
	ON_COMMAND(ID_SYSTEM_CONFIG_DLG, &Cwx_kk_upDlg::OnSystemConfigDlg)
	ON_COMMAND(ID_KK_CONFIG_DLG, &Cwx_kk_upDlg::OnKkConfigDlg)
	ON_COMMAND(ID_START_WATCH, &Cwx_kk_upDlg::OnStartWatch)
	ON_MESSAGE((WM_USER+40), OnLogMessage)
	ON_COMMAND(ID_EXIT, &Cwx_kk_upDlg::OnExit)
	ON_COMMAND(ID_RUN_ON_BACKGROUND, &Cwx_kk_upDlg::OnRunOnBackground)
	ON_COMMAND(ID_CHECK_UPDATE, &Cwx_kk_upDlg::OnCheckUpdate)
END_MESSAGE_MAP()


// Cwx_kk_upDlg 消息处理程序

BOOL Cwx_kk_upDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	dzlog_set_wnd_handle(this->m_hWnd, ZLOG_LEVEL_NOTICE  | ZLOG_LEVEL_ERROR | ZLOG_LEVEL_FATAL );

	Sleep(100);

	Init();

	m_agent.ShowWindow(SW_HIDE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}



void Cwx_kk_upDlg::OnAboutbox()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}


void Cwx_kk_upDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	dzlog_debug("UINT_ID = %d", nID);
	if ((nID & 0xFFF0) == IDM_ABOUTBOX  || nID==ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Cwx_kk_upDlg::OnPaint()
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
HCURSOR Cwx_kk_upDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/*================================================================================================= 
* Init()初始化工作
*
* 主要步骤如下:
*
*  1.ReadConfig();
*  2.InitTrans();初始化车道/车道信息注册
*  3.QuerySyncTime();时间同步 获取服务器时间，然后同步的摄像头
*  4.QueryLimitSpeed();限速同步 
*  5.CamerSDK:SetupDeviceTime 时间校准
*  6.SaveConfig();保存限速信息
*
==================================================================================================== */

void Cwx_kk_upDlg::Init()
{
	CString g_szOcxPath = theApp.m_pszHelpFilePath;
	CString g_szOcxName = theApp.m_pszExeName;

	g_szOcxName += ".HLP";
	int nTmp = g_szOcxName.GetLength();
	nTmp = g_szOcxPath.GetLength() - nTmp;
	g_szOcxPath = g_szOcxPath.Left(nTmp-1);

	m_appPath = g_szOcxPath;//获取本程序运行路径

	ReadConfig();//读取配置信息
	if(systemConfig.autorun==1){//启动后自动监测和上传
		AgentInit();//初始库
		OnStartWatch();//开始工作
	}

	

}

#include <string.h>
#include <stdio.h>
#include <time.h>
#include <dos.h>
////时间同步，设置本机时间
long SetupDeviceTime(const char *csSyncTime)
{
	if(csSyncTime==0)
		return 0;
	if(strlen(csSyncTime)<18)
		return 0;

	char temp[32]={0};
	sprintf(temp, "%s", csSyncTime);
	int csDate[4] = {0};
	int csTime[4] = {0};
	char *pDate = temp;
	char *pTime = 0;
	char *p = 0;

	struct tm tmTime = {0};
	time_t t = 0;
	char errorInfo[256] = {0};

	try{
		pTime = strstr(temp, " ");
		*pTime = '\0';
		pTime ++;

		p = pTime;
		p = strrchr(pTime, ':');
		tmTime.tm_sec = atoi(p+1);//秒 
		*p = '\0';
		p = strrchr(pTime, ':');
		tmTime.tm_min = atoi(p+1);//分
		*p = '\0';
		tmTime.tm_hour = atoi(pTime);//时 

		p = pDate;
		p = strrchr(pDate, '-');
		tmTime.tm_mday = atoi(p+1);//日
		*p = '\0';
		p = strrchr(pDate, '-');
		tmTime.tm_mon = atoi(p+1) - 1;//月
		*p = '\0';
		tmTime.tm_year = atoi(pDate) - 1900;//年


		t=mktime(&tmTime);
		//	stime(&t);// linux
		
		SYSTEMTIME time1;
		GetLocalTime(&time1);
		time1.wYear = tmTime.tm_year + 1900;
		time1.wMonth = tmTime.tm_mon + 1;
		time1.wDay = tmTime.tm_mday;
		time1.wHour = tmTime.tm_hour;
		time1.wMinute = tmTime.tm_min;
		time1.wSecond = tmTime.tm_sec;

		if( SetLocalTime(&time1) )//windows
			sprintf(errorInfo, "时间设置成功:%s",ctime(&t));
		else 
			sprintf(errorInfo, "时间设置失败:%s  ERROR: %s ", csSyncTime, GetLastErrorInfo());
		dzlog_notice(errorInfo);

	}catch(...){
		sprintf(errorInfo,"时间设置失败，时间格式异常:%s  ERROR: %s ", csSyncTime, GetLastErrorInfo());
		dzlog_notice(errorInfo);
		return -1;
	}

	return t;
}

/*================================================================================================= 
* AgentInit()初始化Agent工作
* 1.InitTrans();初始化车道/车道信息注册
* 2.QuerySyncTime();时间同步 获取服务器时间，然后同步的摄像头
* 3.QueryLimitSpeed();限速同步 
* 4.CamerSDK:SetupDeviceTime 时间校准
* 5.SaveConfig();保存限速信息
*
==================================================================================================== */
bool Cwx_kk_upDlg::AgentInit()
{
	TCHAR *m_kkbh = ( kkConfig.id);//卡口编号
	TCHAR * m_fxlx = (kkConfig.direction);//方向类型
	LONG m_cdh = kkConfig.cdid;//车道号
    TCHAR * info = ("<info><jkid>62C01</jkid><jkxlh>791F09090306170309158E9882DB8B8E8FEEF681FF8B9AEB92CFD0A1A1C5A5D3CCA8</jkxlh></info>");//辅助信息
	TCHAR * m_hostport = ( systemConfig.host );//服务器地址
	LONG ret = 1 ;
	if( strcmp(systemConfig.runMode, "wx") ==0)
		ret = m_agent.InitTrans(m_kkbh,m_fxlx,m_cdh, info ,m_hostport);//注册车道

	if(ret<=0){
		sprintf(errorInfo,  "连接服务器失败[%d]",ret);
		dzlog_error(errorInfo);
		MessageBox(errorInfo);
	}

	//时间同步
	CString csSyncTime = "2014-03-02 10:25:50";
	dzlog_notice("时间同步");
	if( strcmp(systemConfig.runMode, "wx") ==0){
		csSyncTime = m_agent.QuerySyncTime();//时间同步
		csSyncTime = csSyncTime.Trim();
		if( !csSyncTime.IsEmpty() )
			SetupDeviceTime(csSyncTime.GetBuffer(csSyncTime.GetLength())); //时间同步，设置本机时间
	}
	
	//限速同步
	char *cllx = "1";//车辆类型	0-大车，1-小车，2-其他车型
	long lLimitSpeed = 40; 
	if( strcmp(systemConfig.runMode, "wx") ==0){
		lLimitSpeed = m_agent.QueryLimitSpeed(m_kkbh,m_fxlx,m_cdh, cllx);////限速同步
	}
	return true;
}

#define R_OK 4 /* Test for read permission. */
#define W_OK 2 /* Test for write permission. */
#define X_OK 1 /* Test for execute permission. */
#define F_OK 0 /* Test for existence. */

//读取配置信息
bool Cwx_kk_upDlg::ReadConfig()
{
	dzlog_notice("读取配置");
	char  cIniFullPath[TF_MAX_PATH_LEN] = {0};
	sprintf(cIniFullPath,"%s\\%s.ini", m_appPath,theApp.m_pszProfileName);

	//	R_OK 只判断是否有读权限 \
		W_OK 只判断是否有写权限 \
		X_OK 判断是否有执行权限 \
		F_OK 只判断是否存在
	if(_access(cIniFullPath, R_OK) == -1)
	{
		sprintf(errorInfo, "读取配置文件失败[%s]", cIniFullPath);
		dzlog_error(errorInfo);
		MessageBox(errorInfo);
		return false;
	}
	
	// get system config from ini
	ini_gets("system", "updateurl", "http://teleframe.xicp.net/update/?update=true&app_name=wx_kk_up", systemConfig.updateurl, TF_MAX_PATH_LEN,  cIniFullPath);
	systemConfig.autorun = ini_getl("system", "autorun",	 1,  cIniFullPath);
	ini_gets("system", "host", "10.2.44.68:9080", systemConfig.host, TF_MAX_PATH_LEN,  cIniFullPath);
	ini_gets("system", "rumMode", "", systemConfig.runMode, TF_MAX_PATH_LEN,  cIniFullPath);

	// get kk config from ini
	ini_gets("kk", "kk.id", "", kkConfig.id, TF_MAX_PATH_LEN,  cIniFullPath);
	ini_gets("kk", "kk.direction", "", kkConfig.direction, TF_MAX_PATH_LEN,  cIniFullPath);
	kkConfig.cdid = ini_getl("kk", "kk.cd.id",  1,  cIniFullPath);
	kkConfig.cdSpeedLimit = ini_getl("kk", "kk.cd.speedLimit",  40,  cIniFullPath);
	kkConfig.cdSpeedLimitDefalut = ini_getl("kk", "kk.cd.speedLimitDefault",  40,  cIniFullPath);
	ini_gets("kk", "kk.ftpPath", "", kkConfig.ftpPath, TF_MAX_PATH_LEN,  cIniFullPath);
	ini_gets("kk", "kk.httpPath", "", kkConfig.httpPath, TF_MAX_PATH_LEN,  cIniFullPath);
	ini_gets("kk", "kk.http", "", kkConfig.http, TF_MAX_PATH_LEN,  cIniFullPath);
	ini_gets("kk", "kk.fileExt", "", kkConfig.fileExt, TF_MAX_PATH_LEN,  cIniFullPath);

	FileUtil::CreateFolders(kkConfig.ftpPath);
	FileUtil::CreateFolders(kkConfig.httpPath);

	//read lasterror
	sprintf(cIniFullPath,"%s\\GetLastError.dat", m_appPath );
	long ErrorSum = ini_getl("error", "sum", 0, cIniFullPath);

	ErrorSum = ErrorSum > MAX_ERROR ? MAX_ERROR : ErrorSum ;

	for(int i=1; i<=ErrorSum; i++)
	{
		sprintf(errorInfo,"%d",i);
		ini_gets("error", errorInfo, "NON", LastError[i], 128,  cIniFullPath);
	}

	dzlog_notice("读取配置文件完成[%s]",cIniFullPath);

	return true;
}

//保存配置信息
bool Cwx_kk_upDlg::SaveConfig()
{
	dzlog_notice("保存配置");

	char  cIniFullPath[TF_MAX_PATH_LEN] = {0};
	sprintf(cIniFullPath,"%s\\%s.ini", m_appPath,theApp.m_pszProfileName);

	//	R_OK 只判断是否有读权限 \
		W_OK 只判断是否有写权限 \
		X_OK 判断是否有执行权限 \
		F_OK 只判断是否存在
	if(_access(cIniFullPath, W_OK) == -1)
	{
		sprintf(errorInfo, "写入配置文件失败,[%s]", cIniFullPath);
		dzlog_error(errorInfo);
		MessageBox(errorInfo);
		return false;
	}

	// put system config to ini
	ini_puts("system", "updateurl",  systemConfig.updateurl,  cIniFullPath);
	ini_putl("system", "autorun",	 systemConfig.autorun,  cIniFullPath);
	ini_puts("system", "host",		 systemConfig.host,  cIniFullPath);
	ini_puts("system", "rumMode",	 systemConfig.runMode,  cIniFullPath);

	// get kk config from ini
	ini_puts("kk", "kk.id",   kkConfig.id,  cIniFullPath);
	ini_puts("kk", "kk.direction",  kkConfig.direction,  cIniFullPath);
	ini_putl("kk", "kk.cd.id",  kkConfig.cdid,  cIniFullPath);
	ini_putl("kk", "kk.cd.speedLimit",  kkConfig.cdSpeedLimit,  cIniFullPath);
	ini_putl("kk", "kk.cd.speedLimitDefault",  kkConfig.cdSpeedLimitDefalut,  cIniFullPath);
	ini_puts("kk", "kk.ftpPath",   kkConfig.ftpPath,  cIniFullPath);
	ini_puts("kk", "kk.httpPath",   kkConfig.httpPath,  cIniFullPath);
	ini_puts("kk", "kk.http",   kkConfig.http,  cIniFullPath);
	ini_puts("kk", "kk.fileExt",   kkConfig.fileExt,  cIniFullPath);

	dzlog_notice("保存配置文件完成[%s]",cIniFullPath);

	return true;
}



//系统配置
void Cwx_kk_upDlg::OnSystemConfigDlg()
{
	dzlog_notice("系统配置");
	systemConfigDlg.setConfig(systemConfig);
	if(systemConfigDlg.DoModal()==IDOK){
		systemConfig = systemConfigDlg.systemConfig;
		dzlog_notice("修改系统配置");
		this->SaveConfig();
	}
}

//卡口配置
void Cwx_kk_upDlg::OnKkConfigDlg()
{
	dzlog_notice("卡口配置");
	kkConfigDlg.pParentDlg = this;
	kkConfigDlg.setConfig(kkConfig);
	if(kkConfigDlg.DoModal()==IDOK){
		kkConfig = kkConfigDlg.kkConfig;	
		dzlog_notice("修改卡口配置");
		this->SaveConfig();
	}
}



//①开始监控目录
//  将监控到的文件放入list
void* ThreadWatcher(void* pParam)
{
	dzlog_notice("[线程开启]开始监控目录 ThreadWatcher  PID : 0x%x  GetLastError(%s)", pthread_self().p, GetLastErrorInfo());
	Cwx_kk_upDlg * dlg = (Cwx_kk_upDlg*)pParam;
	
	char notify[4096]={0}; 
	DWORD cbBytes = 0; 
	TCHAR errorInfo[TF_MAX_PATH_LEN]={0};

	FILE_NOTIFY_INFORMATION *pNotify=(FILE_NOTIFY_INFORMATION *)notify;

	// **** 重要 ****
	// ***  在CreateFile时指定FILE_FLAG_OVERLAPPED标志	\
			ReadDirectoryChangesW时使用lpOverlapped参数
	OVERLAPPED ov;//通知线程退出 ReadDirectoryChangesW
	memset(&ov, 0, sizeof(ov));
    ov.hEvent = CreateEvent(NULL, false, NULL, NULL);

	//	GetCurrentDirectory(MAX_PATH,path.GetBuffer(MAX_PATH+1));
	
	CString str;

	WCHAR  wcFileName[TF_MAX_PATH_LEN]={0};
	DWORD dwFileNameLength ;
	bool bDeleteFileName = true;
	char psTemp[TF_MAX_PATH_LEN]={0};
	long lRet;

//	SetEvent(eventStarted);//release signal
	while (TRUE) 
	{ 
		if(m_ExitCode==1)
			goto end;

		char *cFileName = new char[TF_MAX_PATH_LEN];
		memset(cFileName, 0 , TF_MAX_PATH_LEN);
		memset(wcFileName, 0 , TF_MAX_PATH_LEN);

		bDeleteFileName = true;

	//for (i=0;i<numDirs;i++)
		lRet = ReadDirectoryChangesW( dlg->kkConfig.hDir, &notify, sizeof(notify),
			true, FILE_NOTIFY_CHANGE_LAST_WRITE, 
			&cbBytes,0, NULL);

		if(lRet)
		{ 
			memcpy( wcFileName, pNotify->FileName, pNotify->FileNameLength );
			WideCharToMultiByte( CP_ACP, 0, wcFileName, -1, cFileName, TF_MAX_PATH_LEN, NULL, NULL );

			sprintf( psTemp, "%s/%s", dlg->kkConfig.ftpPath, cFileName );
			if(FileUtil::FindFirstFileExists( psTemp, FILE_ATTRIBUTE_DIRECTORY))
				continue;

			switch(pNotify->Action) 
			{ 
			case FILE_ACTION_ADDED: 
				str.Format("Directory/File added - %s",cFileName);
				break; 
			case FILE_ACTION_REMOVED:
				str.Format("Directory/File removed - %s",cFileName);
				break; 
			case FILE_ACTION_MODIFIED: 
			//	WaitForSingleObject(hMutex,-1);//waiting
				dlg->m_images.push( cFileName );
			//	ReleaseMutex(hMutex);//release 
				str.Format("Directory/File modified - %s",cFileName);
				bDeleteFileName = false;
				break; 
			case FILE_ACTION_RENAMED_OLD_NAME: 
				str.Format("Directory/File rename - %s",cFileName);
				break; 
			case FILE_ACTION_RENAMED_NEW_NAME: 
				str.Format("Directory/File new name - %s",cFileName);
				break; 
			default: 
				break; 
			} 
		}
		dzlog_notice(str);
		if(bDeleteFileName)
			delete cFileName;
	}

end:
	sprintf(errorInfo, "[线程退出]开始监控目录 ThreadWatcher  PID : 0x%x  GetLastError(%s)", pthread_self().p, GetLastErrorInfo());
	dzlog_notice("%s",errorInfo);
	CloseHandle(dlg->kkConfig.hDir);

	pthread_exit(errorInfo);
	return 0;
}

//通过图片名称获取车牌等信息
long  ParseVehicleFromPicture(KKConfig kkConfig, char *imagePath, VehicleInfo &vehicleInfo)
{
	//	图片的命名为： 时间_车牌_号牌种类_车牌颜色_车辆速度.bmp
	//	如: 2014-05-04_16.35.48_京N56Y22_02_2_0.bmp"   "2014-05-04_16.35.48_京N56Y22_02_2_0_moreInfo.bmp"

	//根据图片名称获取图片的时间信息， FORMAT  日期时间_车牌_车牌颜色_车速_违章类型 eg: 2014-1-4_15.02.18_location.jpg
	char fileName[256]={0};
	long lTime=0;
	sprintf( fileName, "%s", (strrchr(imagePath,'\\')+1) );
	//fileName	0x0aa8f6f8 "20140504162515_京N56Y22_02_2_0.bmp"	char [256]
	char *p = strstr(fileName, "_");
	char *pre=0;
	char DateTime[32]={0};
	char *pDateTime = DateTime;
	
	try{
		// Date
		memcpy( pDateTime, fileName, p-fileName);
		pDateTime += (p-fileName);
		*pDateTime = ' ';
		pDateTime ++;

		//Time
		pre = p+1;
		p = strstr(pre, "_");
		memcpy( pDateTime, pre, p-pre);
		pre = pDateTime;
		while( *pre!='\0'){
			if( *pre == '.')
				*pre = ':';
			pre++;
		}
	}catch(...){
		dzlog_error("解析[日期时间失败:%s]", fileName);
	}

	//plate
	char plate[32]={0};
	try{
		pre = p+1;
		p = strstr(pre, "_");
		memcpy(plate, pre, p-pre);
	}catch(...){
		dzlog_error("解析[号牌失败:%s]", fileName);
	}
	
	//hpzl 号牌种类
	char hpzl[8]={0};
	try{
		pre = p+1;
		p = strstr(pre, "_");
		memcpy(hpzl, pre, p-pre);
	}catch(...){
		dzlog_error("解析[号牌种类失败:%s]", fileName);
	}

	//车牌颜色
	char hpys[8]={0};
	try{
		pre = p+1;
		p = strstr(pre, "_");
		memcpy(hpys, pre, p-pre);
	}catch(...){
		dzlog_error("解析[车牌颜色失败:%s]", fileName);
	}
	// ==================================================

//	VehicleInfo *vehicleInfo = new VehicleInfo();
	vehicleInfo.cdh = 1;//车道号
	sprintf(vehicleInfo.kkbh, kkConfig.id);//卡口编号
	sprintf(vehicleInfo.fxlx, kkConfig.direction);//方向类型
	sprintf(vehicleInfo.hphm, plate ); //车牌 无牌、未识别、无法识别均用半角“-”表示，其中无号牌要注意hpzl填41
	sprintf(vehicleInfo.hpzl, hpzl );//号牌种类 参考GA24.7（如01-大型汽车，02-小型汽车，25-农机号牌，41-无号牌，42-假号牌，99-其他号牌），不能为空；
	sprintf(vehicleInfo.hpys, hpys );//号牌颜色	0-白色，1-黄色，2-蓝色，3-黑色，4-绿色，9-其它颜色，不能为空

	sprintf(vehicleInfo.gcsj, DateTime ); //过车时间,e.g: "2003-09-11 11:07:23"
	vehicleInfo.clsd = 0;//车辆速度 最长3位，单位：公里/小时
	vehicleInfo.clxs = kkConfig.cdSpeedLimit;//车辆限速 最长3位，单位：公里/小时
	sprintf(vehicleInfo.wfdm, "1"); //违章类型  违章行为编码 参考GA408.1
	vehicleInfo.cwkc = 0;//车外廓长 最长5位，以厘米为单位
	
	sprintf(vehicleInfo.cllx, "K33");//车辆类型 参考GA24.4（K11-大型普通客车，K21-中型普通客车，K31-小型普通客车，K33-轿车，H11-重型普通客车，H21-中型普通客车，M22-轻便二轮摩托车）
	sprintf(vehicleInfo.fzhphm, "-");//辅助号牌号码	无牌、未识别、无法识别均用半角“-”表示，其中无号牌要注意hpzl填41
	
	sprintf(vehicleInfo.csys, "-");//车身颜色
	sprintf(vehicleInfo.tplj,"%s", kkConfig.http);//通行图片路径  固定部分
	sprintf(vehicleInfo.tp1, "%s",imagePath);//通行图片1  变化的部分
	sprintf(vehicleInfo.tztp, "%s_plate.jpg",imagePath);//通行图片1  变化的部分

	return 1;
}

//②处理监控到的数据
//  获取车牌等信息、检查数据库中是否存在该图片，若存在则继续处理下一个，若不存在则复制图片从ftp到http,写入数据库，删除ftp下的文件
void* ThreadProcessWatchedFiles(void* pParam)
{
	dzlog_notice("[线程开启]处理监控到的数据 ThreadProcessWatchedFiles  PID : 0x%x  GetLastError(%s)", pthread_self().p, GetLastErrorInfo());
	Cwx_kk_upDlg * dlg = (Cwx_kk_upDlg*)pParam;
	TCHAR errorInfo[TF_MAX_PATH_LEN]={0};

	char *imagePath = 0;
	char plateImagePath[512]={0};
	char ftpFilePath[512]={0};
	char ftpPlateFilePath[512]={0};
	char httpFilePath[512]={0};
	char httpPlateFilePath[512]={0};
	bool bExist = false;
	char temp[512]={0};
	long lRet=0;
	char errorValue[512]={0};

	VehicleInfo vehicleInfo={0};
	char *ptr;

	TFDB db;

	while(true){

		if(m_ExitCode==1)
			goto end;
		
		if(dlg->m_images.size()<1){
			dlg->GetDlgItem( IDC_STATUS )->SetWindowText("空闲等待");
			Sleep(10);
			continue;
		}

		sprintf(temp, "正在处理文件队列，剩余文件数量: %d ", dlg->m_images.size() );
		dlg->GetDlgItem( IDC_STATUS )->SetWindowText(temp);

		if(dlg->m_images.empty())
			continue;

		imagePath = dlg->m_images.front();
		dlg->m_images.pop();

		if(imagePath==0)
			continue;

		ptr = strrstr(imagePath, PLATE_END);
		if(ptr>0){
			try{
				*ptr = '\0';
			//	delete 	imagePath;
			}catch(...){	}
		//	continue;
		}
		sprintf( plateImagePath, "%s"PLATE_END, imagePath);//车牌相对路径

		sprintf(ftpFilePath, "%s\\%s", dlg->kkConfig.ftpPath, imagePath);// 过车特写图FTP
		sprintf(httpFilePath, "%s\\%s", dlg->kkConfig.httpPath, imagePath);// 过车特写图HTTP

		sprintf(ftpPlateFilePath,	"%s\\%s"PLATE_END, dlg->kkConfig.ftpPath, imagePath);// 车牌图片FTP
		sprintf(httpPlateFilePath,  "%s\\%s"PLATE_END, dlg->kkConfig.httpPath, imagePath);//车牌图片HTTP

		//check is file 
		if( FileUtil::FindFirstFileExists( ftpFilePath, FALSE) ||
			FileUtil::FindFirstFileExists( ftpPlateFilePath, FALSE) )
		{
			//check can access
			if( ( _access(ftpFilePath, R_OK) == 0 ) ||
				( _access(ftpPlateFilePath, R_OK) == 0 ) )
			{
				lRet = ParseVehicleFromPicture(dlg->kkConfig, imagePath, vehicleInfo);//通过图片名称获取车牌等信息
				if(lRet != 1)
					continue;

				bExist = db.CheckImageExist(imagePath, "tp1");//检查过车图片是否已经分析过,不存在则写入
				if( ! bExist){
					lRet = db.Add(&vehicleInfo);//不存在，则新增到数据库
				}
				//检查是否存在记录【根据车辆图片】
				bExist = db.CheckImageExist(imagePath, "tp1");//新增后需要检查确保成功写入数据库
				if(bExist){
CopyFile:			// 过车特写图
					lRet = FileUtil::CopyFileEx(ftpFilePath, httpFilePath, true);
					if(lRet==true){
						dzlog_error("Copyed file from [%s] to [%s]", ftpFilePath, httpFilePath);
						DeleteFile(ftpFilePath);//处理完成后删除FTP下的文件
					}else{
						dzlog_error("Copy file failed [GetLastError %s] from [%s] to [%s]", GetLastErrorInfo() , ftpFilePath, httpFilePath);
					}
				}
				//检查是否存在记录【根据车牌图片】
				bExist = db.CheckImageExist(plateImagePath, "tztp");//新增后需要检查确保成功写入数据库
				if(bExist){
CopyPlateFile:		///车牌图片
					lRet = FileUtil::CopyFileEx(ftpPlateFilePath, httpPlateFilePath, true);
					if(lRet==true){
						dzlog_error("Copyed file from [%s] to [%s]", ftpFilePath, httpFilePath);
						DeleteFile(ftpPlateFilePath);//处理完成后删除FTP下的文件
					}else{
						dzlog_error("Copy file failed [GetLastError %s] from [%s] to [%s]", GetLastErrorInfo() , ftpFilePath, httpFilePath);
					}
				}
			}else{
				dzlog_error("cannot access file [%s]  [GetLastError %s] ", ftpFilePath,  GetLastErrorInfo());
			}
		}

		try{
			if(imagePath)
				delete imagePath;
		}catch(...){	}

		Sleep(10);
	}

end:
	sprintf(errorInfo, "[线程退出]处理监控到的数据 ThreadProcessWatchedFiles  PID : 0x%x  GetLastError(%s)", pthread_self().p, GetLastErrorInfo());
	dzlog_notice("%s",errorInfo);
	pthread_exit(errorInfo);
	return 0;
}


//③上传数据
//  读取数据库，获取未上传的图片进行上传，如果http下的图片文件不存在，则读取ftp下的图片并进行上传
void* ThreadUpload(void* pParam)
{
	dzlog_notice("[线程开启]上传数据 ThreadUpload  PID : 0x%x  GetLastError(%s)", pthread_self().p, GetLastErrorInfo());
	Cwx_kk_upDlg * dlg = (Cwx_kk_upDlg*)pParam;
	TCHAR errorInfo[TF_MAX_PATH_LEN]={0};

	queue<VehicleInfo*> listIn;
	VehicleInfo *vehicleInfo = 0;
	long lRet = 0;
	int id = 0;
	bool connected = false;
	TFDB db;

	while(true){

		db.Query(listIn);// 查询未上传的数据

next:
		if(m_ExitCode==1)
			goto end;

		if(listIn.empty()){
			Sleep(100);
			continue;
		}

		vehicleInfo = listIn.front();
		listIn.pop();

		if(vehicleInfo==0)
			goto next;

connect:
		if( !connected )
			lRet = dlg->InitTrans(vehicleInfo);//初始化连接，注册车道

		if( lRet == CONNECT_ERROR){
			dzlog_error("网络连接失败") ;
			connected = false;
			Sleep(2000);
			goto connect;
		}	
		connected = true;

		id = vehicleInfo->id;
		lRet = dlg->UploadInfo(vehicleInfo);//上传数据
		if(vehicleInfo)
			delete vehicleInfo;

		if( lRet == OK)
			db.Uploaded( id );//上传成功
		else{
			//上传失败
			if( lRet == FALIED) { //连接成功但是上传失败
				dzlog_error("连接成功但是上传失败 @ id=%d ", id) ; // to do something
			}else if( lRet == CONNECT_ERROR){
				dzlog_error("网络连接失败 @ id=%d ",  id) ;
				connected = false;
				Sleep(100);
				goto connect; //重新连接
			}	
		}
		goto next;

		Sleep(100);
	}
end:
	sprintf(errorInfo, "[线程退出]上传数据 ThreadUpload PID : 0x%x  GetLastError(%s)", pthread_self().p, GetLastErrorInfo());
	dzlog_notice("%s",errorInfo);
	pthread_exit(errorInfo);
	return 0;
}


#define MAX_THREAD 10
pthread_t thread[MAX_THREAD]={0};  
void*	pth_join_ret[MAX_THREAD];


/* =========================================================================
*  余留文件处理线程
*  处理上次退出后FTP未处理的文件
*    若程序死掉了，自己重启后首先处理上次余留的FTP上传的文件
*    1.用FileUtil::ListFiles获取FTP目录下未处理的文件
*    2.放入list队列 
===========================================================================*/
void* ThreadProcessLast(void *pParam)
{
	dzlog_notice("[线程开启]余留文件处理线程 ThreadProcessLast  PID : 0x%x GetLastError(%s)", pthread_self().p, GetLastErrorInfo());
	Cwx_kk_upDlg * dlg = (Cwx_kk_upDlg*)pParam;
	TCHAR errorInfo[TF_MAX_PATH_LEN]={0};
	
	list<char *> list;
	char *imagePath = 0;

	while(true){
		if(m_ExitCode==1)
			goto end;

		FileUtil::ListFiles(dlg->kkConfig.ftpPath, NULL, list, dlg->kkConfig.fileExt, 1, false, true);

		while( !list.empty()){
			if(m_ExitCode==1)
				goto end;
			imagePath = list.front();
			list.pop_front();
			if(imagePath!=0 && dlg->m_images.size()<500)
				dlg->m_images.push(imagePath);
		}
		Sleep(10*1000);
	}
end:
	sprintf(errorInfo, "[线程退出]余留文件处理线程 ThreadProcessLast PID : 0x%x GetLastError(%s)", pthread_self().p, GetLastErrorInfo());
	dzlog_notice("%s",errorInfo);
	pthread_exit(errorInfo);
	return 0;
}


typedef void (*_UpdateShareTime)(); //宏定义函数指针类型
typedef time_t (*_GetNowTime)();
typedef time_t (*_GetShareTime)();


//系统资源监控
void* ProcessMonitorThread(void *pParam)
{
start:
	dzlog_notice("[线程开启]系统资源监控线程 ThreadProcessLast  PID : 0x%x GetLastError(%s)", pthread_self().p, GetLastErrorInfo());
	Cwx_kk_upDlg * dlg = (Cwx_kk_upDlg*)pParam;
	TCHAR errorInfo[TF_MAX_PATH_LEN]={0};

	char temp[512]={0};
	uint64_t  mem=0;//内存使用
	uint64_t  vmem=0;//虚拟内存使用
	int cpu =0;
	int ret=0;
	char tempsize[64]={0};
	time_t t=0;

	time_t	ShareTime = 0;
	time_t	NowTime = 0;
	char	pNowTime[64] = {0}; 
	char	pShareTime[64] = {0}; 

	HINSTANCE hDll; //DLL句柄 
	_UpdateShareTime	UpdateShareTime; //函数指针
	_GetNowTime			GetNowTime;
	_GetShareTime		GetShareTime;
	hDll = LoadLibrary("ShareModule.dll");
	if (hDll == NULL){
		dzlog_error(" hDll = LoadLibrary(ShareModule.dll) failed");
		return 0;
	}

	UpdateShareTime = (_UpdateShareTime)GetProcAddress(hDll, "UpdateShareTime");
	if (UpdateShareTime == NULL){
		FreeLibrary(hDll);
		dzlog_error(" hDll = GetProcAddress(UpdateShareTime) failed");
		return 0;
	}
	GetNowTime = (_GetNowTime)GetProcAddress(hDll, "GetNowTime");
	GetShareTime  = (_GetShareTime)GetProcAddress(hDll, "GetShareTime");

	try
	{
		while( true )
		{
			if(m_ExitCode==1)
				goto end;

		//	UpdateShareTime();//设置存活时间

			NowTime = GetNowTime();
			sprintf(pNowTime,"%s", asctime(localtime(&NowTime)));
			if( strlen(pNowTime) > 2)
				pNowTime[strlen(pNowTime)-1] = '\0';

			ShareTime = GetShareTime();
			sprintf(pShareTime,"%s", asctime(localtime(&ShareTime)));
			if( strlen(pShareTime) > 2)
				pShareTime[strlen(pShareTime)-1] = '\0';

			sprintf(errorInfo, "ShareTime = %d [ %s ]  NowTime = %d [ %s ]  diff = %d ", (long)ShareTime, pShareTime, (long)NowTime, pNowTime, NowTime-ShareTime );
		//	dlg->GetDlgItem(ID_PROCESS_STATE)->SetWindowText(errorInfo);

			cpu = get_cpu_usage();
			ret = get_memory_usage( &mem, &vmem);
			if( cpu>=0)
				sprintf(temp, "CPU: %d%% ", cpu );
			if( ret>=0){
				sprintf(temp, "%s  Memery: %s | %s ",temp, SizeFormat(mem, tempsize) , SizeFormat(vmem, tempsize) );
			}
			sprintf(errorInfo, "系统资源 ProcessMonitorThread [%s] %s ",temp, GetLastErrorInfo());
			dzlog_info("%s", errorInfo);
			dlg->GetDlgItem(ID_PROCESS_STATE)->SetWindowText(temp);
			Sleep(1000);
		}

	}catch(...){
		sprintf(errorInfo, "[线程退出]系统资源监控线程 ProcessMonitorThread PID : 0x%x GetLastError(%s)", pthread_self().p, GetLastErrorInfo());
		dzlog_notice("%s",errorInfo);
		goto start;
	}

end:
	sprintf(errorInfo, "[线程退出]系统资源监控线程 ProcessMonitorThread PID : 0x%x GetLastError(%s)", pthread_self().p, GetLastErrorInfo());
	dzlog_notice("%s",errorInfo);
	pthread_exit(errorInfo);
	return 0;
}

//开始工作线程
void * ThreadStartAll(void *pParam)
{
	TCHAR errorInfo[TF_MAX_PATH_LEN]={0};

	threadRuning = !threadRuning;

	Cwx_kk_upDlg * dlg = (Cwx_kk_upDlg*)pParam;
	CMenu *pMenu = AfxGetApp()->m_pMainWnd->GetMenu(); 
	CMenu *pSubMenu = pMenu->GetSubMenu(2);

	dzlog_notice("======================================================================");
	if(threadRuning){
		dzlog_notice("开始工作线程 StartWatch");
		m_ExitCode = 0;
		pthread_create(&thread[0], NULL, ThreadProcessLast, pParam);//余留文件处理线程
		pthread_create(&thread[1], NULL, ThreadWatcher, pParam); //开启监控目录线程
		pthread_create(&thread[2], NULL, ThreadProcessWatchedFiles, pParam); //开启处理监控到的数据处理线程
		pthread_create(&thread[3], NULL, ThreadUpload, pParam); //开启数据上传线程
		pthread_create(&thread[4], NULL, ProcessMonitorThread, pParam); //系统资源监控

		//AfxBeginThread(ThreadWatcher, this);//开启监控目录线程
		//AfxBeginThread(ThreadProcessWatchedFiles, this);//开启处理监控到的数据处理线程
		//AfxBeginThread(ThreadUpload, this);//开启数据上传线程
		pSubMenu->ModifyMenu(ID_START_WATCH, MF_BYCOMMAND ,ID_START_WATCH,"关闭监控");
	}else{
		dzlog_notice("关闭工作线程 OnStartWatch");
		m_ExitCode = 1;
		Sleep(500);
		pthread_join( thread[0], &pth_join_ret[0]);
		dzlog_notice("关闭线程 thread0[余留文件处理线程]   retruns : %s ", pth_join_ret[0]);
		pthread_join( thread[1], &pth_join_ret[1]);
		dzlog_notice("关闭线程 thread1[监控目录线程]   retruns : %s ", pth_join_ret[1]);
		pthread_join( thread[2], &pth_join_ret[2]);
		dzlog_notice("关闭线程 thread2[数据处理线程]   retruns : %s ", pth_join_ret[2]);
		pthread_join( thread[3], &pth_join_ret[3]);
		dzlog_notice("关闭线程 thread3[数据上传线程]   retruns : %s ", pth_join_ret[3]);
		pthread_join( thread[4], &pth_join_ret[4]);
		dzlog_notice("关闭线程 thread4[系统资源监控线程]   retruns : %s ", pth_join_ret[4]);

	//	
		pSubMenu->ModifyMenu(ID_START_WATCH, MF_BYCOMMAND ,ID_START_WATCH,"开启监控");
	}
	sprintf(errorInfo, "[线程退出] 开始工作线程 ThreadStartAll  PID : 0x%x ", pthread_self() );
	pthread_exit(errorInfo);

	return 0;
}



//开始监控目录
void Cwx_kk_upDlg::OnStartWatch()
{
	//
	if( !threadRuning){
		
		FileUtil::CreateFolders(kkConfig.ftpPath);
		FileUtil::CreateFolders(kkConfig.httpPath);

		kkConfig.hDir = CreateFile( kkConfig.ftpPath ,
			FILE_LIST_DIRECTORY,
			FILE_SHARE_READ |
			FILE_SHARE_WRITE |
			FILE_SHARE_DELETE,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
			NULL);

		if( kkConfig.hDir == INVALID_HANDLE_VALUE )
		{
			sprintf(errorInfo, "打开监控目录失败[%s][%s]",kkConfig.ftpPath, GetLastErrorInfo() );
			dzlog_error(errorInfo);
			MessageBox(errorInfo);
			return ;
		}

	}

	pthread_t thread;
	pthread_create(&thread, NULL, ThreadStartAll, this); //开启工作线程
}
void Cwx_kk_upDlg::OnCancel()
{
//	SetEvent(ov.hEvent);  //使可以退出
	OnExit();
}

/*	初始化上传接口
*
*		#define		CONNECTED	2  //连接成功
*		#define		CONNECT_ERROR	3 //连接失败
*/
long Cwx_kk_upDlg::InitTrans(VehicleInfo *vehicleInfo)
{
	TCHAR *m_kkbh = ( vehicleInfo->kkbh );//卡口编号
	TCHAR * m_fxlx = (vehicleInfo->fxlx);//方向类型
	LONG m_cdh = vehicleInfo->cdh;//车道号
	TCHAR * info = ("<info><jkid>62C01</jkid><jkxlh>791F09090306170309158E9882DB8B8E8FEEF681FF8B9AEB92CFD0A1A1C5A5D3CCA8</jkxlh></info>");//辅助信息
	TCHAR * m_hostport = ( systemConfig.host );//服务器地址
	LONG lRet = 1 ;
	if( strcmp(systemConfig.runMode, "wx") ==0)
		lRet = m_agent.InitTrans(m_kkbh,m_fxlx,m_cdh, info ,m_hostport);//注册车道

	if( lRet <= 0){
		dzlog_error("连接服务器失败[%d] [GetLastError:%s] ", lRet, GetLastErrorInfo());

		return CONNECT_ERROR;
	}
	return CONNECTED;
}

/*
* 上传数据
* return: 
*	OK : OK
*	CONNECT_ERROR : connect server error
*   FALIED : connected, but upload failed
*
*	Ref:
		#define		OK			1  //成功
		#define		FALIED		-1 //失败
		#define		CONNECTED	2  //连接成功
		#define		CONNECT_ERROR	3 //连接失败
*/
long Cwx_kk_upDlg::UploadInfo(VehicleInfo *vehicleInfo)
{

	long lRet = OK;
	char filePath[512]={0};
	
	if( strcmp(systemConfig.runMode, "wx") ==0)
	{
		//上传文本
		lRet = m_agent.WriteVehicleInfo(
			vehicleInfo->kkbh, //卡口编号
			vehicleInfo->fxlx, //	方向类型
			vehicleInfo->cdh, //	车道号
			vehicleInfo->hphm,//	号牌号码
			vehicleInfo->hpzl,//	号牌种类
			vehicleInfo->gcsj,//	过车时间
			vehicleInfo->clsd, //	车辆速度
			vehicleInfo->clxs, //	车辆限速
			vehicleInfo->wfdm,//	违章行为编码
			vehicleInfo->cwkc, //车外廓长
			vehicleInfo->hpys, //	号牌颜色
			vehicleInfo->cllx,//	车辆类型
			"","","","","","",
			vehicleInfo->tztp,//	通行图片路径
			vehicleInfo->tp1,//	通行图片1
			"", "", "");
		if(lRet<1)
			return lRet;

		//上传过车图片
		sprintf(filePath, "%s\\", kkConfig.httpPath, vehicleInfo->tp1);
		if( !FileUtil::FindFirstFileExists( filePath, false) ){
			sprintf(filePath, "%s\\", kkConfig.ftpPath, vehicleInfo->tp1);
		}
		if( !FileUtil::FindFirstFileExists( filePath, false) ){
			dzlog_error("过车图片不存在: %s", vehicleInfo->tp1);
		}else{
			lRet = m_agent.SavePicture(
				vehicleInfo->kkbh, //卡口编号
				vehicleInfo->fxlx, //	方向类型
				vehicleInfo->cdh, //	车道号
				vehicleInfo->hphm,//	号牌号码
				vehicleInfo->gcsj,//	过车时间
				1, // 图片序号	1-图片1，2-图片2，3-图片3
				filePath  //本地图片
			);
		}
		//上传特征图片，号牌图片
		sprintf(filePath, "%s\\", kkConfig.httpPath, vehicleInfo->tztp);
		if( !FileUtil::FindFirstFileExists( filePath, false) ){
			sprintf(filePath, "%s\\", kkConfig.ftpPath, vehicleInfo->tztp);
		}
		if( !FileUtil::FindFirstFileExists( filePath, false) ){
			dzlog_error("号牌图片不存在: %s", vehicleInfo->tztp);
		}else{
			lRet = m_agent.SaveTzPicture(
				vehicleInfo->kkbh, //卡口编号
				vehicleInfo->fxlx, //	方向类型
				vehicleInfo->cdh, //	车道号
				vehicleInfo->hphm,//	号牌号码
				vehicleInfo->gcsj,//	过车时间
				filePath  //本地图片
			);
		}
	}
	return lRet;
}

//显示log
bool  InitedLogGUI = false;
char lastMsg[2048] = {0}; 
LRESULT Cwx_kk_upDlg::OnLogMessage(WPARAM wParam, LPARAM lParam)
{
	if( !InitedLogGUI )
	{
		m_LogList.SendMessage (LB_SETHORIZONTALEXTENT , 4096, 0);
		m_LogList.EnableScrollBarCtrl(TRUE);
		InitedLogGUI = true;
	}
	long curIndex = m_LogList.GetCurSel();
	long count = m_LogList.GetCount(); 

	LPCTSTR lpsz = (LPCTSTR)wParam;

	OutputDebugString(lpsz);
	char* pReturnChar = (char *)strchr(lpsz,'\r');
	if(pReturnChar != NULL)
		*pReturnChar = 0;

	
	// set the message bar text
	if(m_LogList.GetCount()>2000)
		m_LogList.ResetContent();
	if (lpsz != 0)
	{
		if( strcmp(lpsz, lastMsg) == 0 )
			;//去除重复的消息
		else
		{
			m_LogList.AddString(lpsz);
			//若用户为选择其它消息，则选择最新消息
			sprintf(lastMsg, "%s", lpsz);
			if(curIndex==(count-1))
				m_LogList.SetCurSel(count);//选择最新消息
		}
	}

	return 0;

}

void Cwx_kk_upDlg::OnExit()
{
	//退出
	if(threadRuning)
		OnStartWatch();

	::PostQuitMessage(0);
}

void Cwx_kk_upDlg::OnRunOnBackground()
{
	//后台运行
}

#include <winsock2.h>                   //socket头文件
#pragma comment (lib, "ws2_32")         //socket库文件

#include "TCode.h" 

//检查更新
void *ThreadUpdate(void *pParam)
{
	dzlog_notice("[线程开启]系统资源监控线程 ThreadProcessLast  PID : 0x%x GetLastError(%s)", pthread_self().p, GetLastErrorInfo());
	Cwx_kk_upDlg * dlg = (Cwx_kk_upDlg*)pParam;
	TCHAR errorInfo[TF_MAX_PATH_LEN]={0};

	// 1. socket 
#ifdef  USE_SOCKET

	long lRet;
	WSADATA wsaData;
    lRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (lRet)
	{
		sprintf(errorInfo, "WSAStartup() Error: %s\n", GetLastErrorInfo());
		WSACleanup();
		goto end;
	}

	sockaddr_in sockaddr;

	IN_ADDR  iaHost;
	LPHOSTENT lpHostEntry;

	iaHost.s_addr = inet_addr( dlg->systemConfig.updateurl );
	if (iaHost.s_addr == INADDR_NONE)
	{
		// Wasn't an IP address string, assume it is a name
		lpHostEntry = gethostbyname(dlg->systemConfig.updateurl);
	}
	else
	{
		// It was a valid IP address string
		lpHostEntry = gethostbyaddr((const char *)&iaHost,
			sizeof(struct in_addr), AF_INET);
	}

	if (lpHostEntry == NULL)
	{
		sprintf(errorInfo, "gethostbyname() |gethostbyaddr() Error: %s ", GetLastErrorInfo());
		goto end;
	}

	
	// 
	// Create a TCP/IP stream socket
	//
	SOCKET Socket; 

	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Socket == INVALID_SOCKET)
	{
		sprintf(errorInfo, "Error socket() %s ", GetLastErrorInfo());
		goto end;
	}

	//
	// Find the port number for the HTTP service on TCP
	//
	LPSERVENT lpServEnt;
	SOCKADDR_IN saServer;

	lpServEnt = getservbyname("http", "tcp");
	if (lpServEnt == NULL)
		saServer.sin_port = htons(80);
	else
		saServer.sin_port = lpServEnt->s_port;


	//
	// Fill in the rest of the server address structure
	//
	saServer.sin_family = AF_INET;
	saServer.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list);


	//
	// Connect the socket
	//
	int nRet;

	nRet = connect(Socket, (LPSOCKADDR)&saServer, sizeof(SOCKADDR_IN));
	if (nRet == SOCKET_ERROR)
	{
		sprintf(errorInfo, "Error connect() %s ", GetLastErrorInfo());
		closesocket(Socket);
		goto end;
	}


	//
	// Format the HTTP request
	//
	char szBuffer[1024];

//	sprintf(szBuffer, "GET %s\n", lpFileName);
	nRet = send(Socket, szBuffer, strlen(szBuffer), 0);
	if (nRet == SOCKET_ERROR)
	{
		sprintf(errorInfo, "Error send()  %s ", GetLastErrorInfo());
		closesocket(Socket); 
		goto end;
	}

	//
	// Receive the file contents and print to stdout
	//
	while(1)
	{
		// Wait to receive, nRet = NumberOfBytesReceived
		nRet = recv(Socket, szBuffer, sizeof(szBuffer), 0);
		if (nRet == SOCKET_ERROR)
		{
			sprintf(errorInfo, "Error recv()  %s ", GetLastErrorInfo());
			break;
		}

		fprintf(stderr,"\nrecv() returned %d bytes", nRet);

		// Did the server close the connection?
		if (nRet == 0)
			break;
		// Write to stdout
		fwrite(szBuffer, nRet, 1, stdout);
	}
	closesocket(Socket); 

#else
	// user httpclient

	//	1：get方式   
	CHttpClient   conn;   
	CString   value;
	value.Format( "%s", dlg->systemConfig.updateurl );    
	value=conn.doGet(value);  
	value = U2G(value);
	dzlog_notice("CHttpClient Recv:  %s", value);

	//2：post方式   
	/*
	CHttpClient   conn;   
	CString   value;
	value.Format( "%s", dlg->systemConfig.updateurl );    
	conn.addParam("name1","value1");   
	conn.addParam("name2","value2");   
	conn.addParam("name3","value3");   
	conn.addParam("name4","value4");   
	conn.addParam("name5","value5");   
	conn.addParam("name6","value6");   
	value=conn.doPost(value);         
	*/

#endif

	sprintf(errorInfo, "[线程退出] ThreadUpdate PID : 0x%x GetLastError(%s)", pthread_self().p, GetLastErrorInfo());

end:
	dzlog_notice("%s",errorInfo);
	pthread_exit(errorInfo);

	return 0;
}
void Cwx_kk_upDlg::OnCheckUpdate()
{
	//检查更新
	pthread_t thread;
	pthread_create(&thread, NULL, ThreadUpdate, this);//余留文件处理线程

}

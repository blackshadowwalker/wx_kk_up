// KKConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "wx_kk_up.h"
#include "KKConfigDlg.h"



// KKConfigDlg 对话框

IMPLEMENT_DYNAMIC(KKConfigDlg, CDialog)

KKConfigDlg::KKConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(KKConfigDlg::IDD, pParent)
	, id(_T(""))
	, direction(_T(""))
	, cdid(0)
	, cdSpeedLimit(0)
	, cdSpeedLimitDefalut(0)
	, path(_T(""))
	, m_httpPath(_T(""))
	, http(_T(""))
	, fileExt(_T("\"*.jpg;*.bmp;*.png\""))
{
	pParentDlg = 0;
//	dzlog_info("init KKConfigDlg pParent: 0x%x ", pParent);

}

KKConfigDlg::~KKConfigDlg()
{
}

void KKConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, id);
	DDX_Text(pDX, IDC_EDIT2, direction);
	DDX_Text(pDX, IDC_EDIT3, cdid);
	DDV_MinMaxLong(pDX, cdid, 1, 10);
	DDX_Text(pDX, IDC_EDIT4, cdSpeedLimit);
	DDV_MinMaxLong(pDX, cdSpeedLimit, 0, 1000);
	DDX_Text(pDX, IDC_EDIT5, cdSpeedLimitDefalut);
	DDV_MinMaxLong(pDX, cdSpeedLimitDefalut, 0, 1000);
	DDX_Text(pDX, IDC_EDIT6, path);
	DDX_Text(pDX, IDC_EDIT7, m_httpPath);
	DDX_Text(pDX, IDC_EDIT8, http);
	DDX_Text(pDX, IDC_EDIT9, fileExt);
}

BEGIN_MESSAGE_MAP(KKConfigDlg, CDialog)
	ON_BN_CLICKED(IDOK, &KKConfigDlg::OnBnClickedOk)
	ON_BN_CLICKED(BT_BROWSER_FOLDER, &KKConfigDlg::OnBnClickedBrowserFolder)
	ON_BN_CLICKED(BT_BROWSER_FOLDER2, &KKConfigDlg::OnBnClickedBrowserFolder2)
	ON_BN_CLICKED(IDC_BT_QUERY_SPEEDLIMIT, &KKConfigDlg::OnBnClickedBtQuerySpeedlimit)
END_MESSAGE_MAP()


// KKConfigDlg 消息处理程序

void KKConfigDlg::OnBnClickedOk()
{
	UpdateData(true);

	sprintf( kkConfig.id, id.GetBuffer(id.GetLength()));
	sprintf( kkConfig.direction, direction.GetBuffer(direction.GetLength()));
	kkConfig.cdid = cdid;
	kkConfig.cdSpeedLimit = cdSpeedLimit;
	kkConfig.cdSpeedLimitDefalut = cdSpeedLimitDefalut;
	sprintf( kkConfig.ftpPath, path);
	sprintf( kkConfig.httpPath, m_httpPath);
	sprintf( kkConfig.http , http);
	sprintf( kkConfig.fileExt, fileExt);

	OnOK();
}

BOOL KKConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	id.Format("%s", kkConfig.id);
	direction.Format("%s", kkConfig.direction);
	cdid = kkConfig.cdid ;
	cdSpeedLimit = kkConfig.cdSpeedLimit;
	cdSpeedLimitDefalut = kkConfig.cdSpeedLimitDefalut;
	path.Format("%s", kkConfig.ftpPath);
	m_httpPath.Format("%s", kkConfig.httpPath);
	http.Format("%s", kkConfig.http);
	fileExt.Format("%s", kkConfig.fileExt);

	UpdateData(false);

	return TRUE;  
}

#include "FileUtil.h"

void KKConfigDlg::OnBnClickedBrowserFolder()
{
	UpdateData(true);
	char *temp = FileUtil::SelectFolder(this->m_hWnd, "", path.GetBuffer(path.GetLength()) );
	if(temp!=0){
		path.Format("%s", temp);
	}
	UpdateData(false);
}

void KKConfigDlg::OnBnClickedBrowserFolder2()
{
	UpdateData(true);
	char *temp = FileUtil::SelectFolder(this->m_hWnd, "", m_httpPath.GetBuffer(m_httpPath.GetLength()) );
	if(temp!=0){
		m_httpPath.Format("%s", temp);
	}
	UpdateData(false);
}


#include "wx_kk_upDlg.h"

void KKConfigDlg::OnBnClickedBtQuerySpeedlimit()
{
	// 查询限速值
	if(pParentDlg==0)
		return ;

	Cwx_kk_upDlg* pDlg = (Cwx_kk_upDlg *)pParentDlg;

	char errorInfo[256]={0};

	UpdateData(true);

	sprintf( kkConfig.id, id.GetBuffer(id.GetLength()));//卡口编号
	sprintf( kkConfig.direction, direction.GetBuffer(direction.GetLength()));//方向
	kkConfig.cdid = cdid;//车道号
	kkConfig.cdSpeedLimit = cdSpeedLimit;//限速
	kkConfig.cdSpeedLimitDefalut = cdSpeedLimitDefalut;//限速默认值

	TCHAR *m_kkbh = ( kkConfig.id);//卡口编号
	TCHAR * m_fxlx = (kkConfig.direction);//方向类型
	LONG m_cdh = kkConfig.cdid;//车道号
    TCHAR * info = ("<info><jkid>62C01</jkid><jkxlh>791F09090306170309158E9882DB8B8E8FEEF681FF8B9AEB92CFD0A1A1C5A5D3CCA8</jkxlh></info>");//辅助信息
	TCHAR * m_hostport = ( pDlg->systemConfig.host );//服务器地址
	LONG ret = 1 ;
	if( strcmp(pDlg->systemConfig.runMode, "wx") ==0)
		ret = pDlg->m_agent.InitTrans(m_kkbh,m_fxlx,m_cdh, info ,m_hostport);//注册车道
	
	if(ret<=0){
		sprintf(errorInfo,  "连接服务器失败[%d]",ret);
		dzlog_error(errorInfo);
		MessageBox(errorInfo);
	}

	/*
	//时间同步
	CString csSyncTime = "";
	if( strcmp(systemConfig.runMode, "wx") ==0)
		csSyncTime = m_agent.QuerySyncTime();//时间同步
	//SetupDeviceTime()
	*/
	
	//限速同步
	TCHAR *cllx = "1";//车辆类型	0-大车，1-小车，2-其他车型
	long lLimitSpeed = 40; 
	if( strcmp(pDlg->systemConfig.runMode, "wx") ==0){
		lLimitSpeed = pDlg->m_agent.QueryLimitSpeed(m_kkbh,m_fxlx,m_cdh, cllx);////限速同步
		if( lLimitSpeed < 5 ) //为了整个系统的鲁棒性考虑，如果函数返回的限速值为0公里(或小于5公里)，建议使用本地缺省的限速值。这往往意味着中心库没有准确记录限速值。
			cdSpeedLimit = kkConfig.cdSpeedLimitDefalut;
		else
			cdSpeedLimit = lLimitSpeed;
	}
	UpdateData(true);

}

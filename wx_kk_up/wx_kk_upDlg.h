
// wx_kk_upDlg.h : 头文件
//

#pragma once

#ifndef _WX_KK_UPDLG_HEADER_
#define _WX_KK_UPDLG_HEADER_

#include "htagentobj1.h"

#include "systemconfigdlg.h"
#include "kkconfigdlg.h"

#include "queue"
#include "afxwin.h"
using namespace std;

// Cwx_kk_upDlg 对话框
class Cwx_kk_upDlg : public CDialog
{
// 构造
public:
	Cwx_kk_upDlg(CWnd* pParent = NULL);	// 标准构造函数
	~Cwx_kk_upDlg();

// 对话框数据
	enum { IDD = IDD_WX_KK_UP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CHtagentobj1 m_agent;

	void Init();//初始化，读取配置文件->InitTrans()->QuerySyncTime()->QueryLimitSpeed-> DH_SDK::ENT_SetupDeviceTime设备校时
	bool AgentInit();
	bool ReadConfig();
	bool SaveConfig();
	
	CString m_appPath;//本程序运行路径

	static SystemConfig	systemConfig;//系统环境设置
	static KKConfig		kkConfig;//卡口配置
	VehicleInfo		vehicleInfo;
	queue<char* >  m_images;
	

	afx_msg void OnAboutbox();
	afx_msg void OnSystemConfigDlg();
	afx_msg void OnKkConfigDlg();

	SystemConfigDlg systemConfigDlg;
	KKConfigDlg		kkConfigDlg;
	afx_msg void OnStartWatch();
	
	TCHAR errorInfo[TF_MAX_PATH_LEN] ;

	long InitTrans(VehicleInfo *vehicleInfo);

	/*
	* 上传数据
	* return: 
	*	1 : OK
	*	2 : connect server error
	*   3 : connected, but upload failed
	*/
	long UploadInfo(VehicleInfo *vehicleInfo);

	afx_msg LRESULT OnLogMessage(WPARAM wParam, LPARAM lParam = 0L);

protected:
	virtual void OnCancel();
public:
	CListBox m_LogList;
	afx_msg void OnExit();
	afx_msg void OnRunOnBackground();
	afx_msg void OnCheckUpdate();
};

#endif 
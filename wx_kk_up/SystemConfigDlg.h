#pragma once
#include "afxwin.h"


// SystemConfigDlg 对话框

class SystemConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(SystemConfigDlg)

public:
	SystemConfigDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~SystemConfigDlg();

// 对话框数据
	enum { IDD = IDD_SYS_CONFIG_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public :
	SystemConfig systemConfig;
	void  setConfig(SystemConfig config);

	afx_msg void OnBnClickedOk();
	CString m_updateurl;
	CComboBox m_autorun;
	CString m_host;
	CComboBox m_devMode;
	virtual BOOL OnInitDialog();
	
};

#pragma once


// KKConfigDlg 对话框




class KKConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(KKConfigDlg)

	

public:
	KKConfigDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~KKConfigDlg();

	CWnd *pParentDlg;

// 对话框数据
	enum { IDD = IDD_KK_CONFIG_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public :
	KKConfig		kkConfig;
	void	setConfig(KKConfig config){ this->kkConfig = config; }

	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

	CString id;//卡口编号
	CString direction;// 方向类型
	long cdid;// 车道号
	long cdSpeedLimit;// 车道限速
	long cdSpeedLimitDefalut;// 车道限速默认值
	CString path;// 监控目录

	afx_msg void OnBnClickedBrowserFolder();
	CString m_httpPath;
	afx_msg void OnBnClickedBrowserFolder2();
	afx_msg void OnBnClickedBtQuerySpeedlimit();
	CString http;
	CString fileExt;
};

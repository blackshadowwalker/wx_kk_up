
// wx_kk_up.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号

#include "zlog.h"
#include "pthread.h"
#include "FileUtil.h"

#include "CrashHandler.h"

#include "sqlite3.h"
#include "mysql.h"
#include "md5.h"
#include "tfcommon.h"


// Cwx_kk_upApp:
// 有关此类的实现，请参阅 wx_kk_up.cpp
//

class Cwx_kk_upApp : public CWinAppEx
{
public:
	Cwx_kk_upApp();

// 重写
	public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern Cwx_kk_upApp theApp;
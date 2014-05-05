#pragma once

// 计算机生成了由 Microsoft Visual C++ 创建的 IDispatch 包装类

// 注意: 不要修改此文件的内容。如果此类由
//  Microsoft Visual C++ 重新生成，您的修改将被覆盖。

/////////////////////////////////////////////////////////////////////////////
// CHtagentobj1 包装类

class CHtagentobj1 : public CWnd
{
protected:
	DECLARE_DYNCREATE(CHtagentobj1)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0xD681E3B, 0xB2A0, 0x498F, { 0x9A, 0xDA, 0xC, 0xA2, 0x6, 0xB2, 0x33, 0xA5 } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
						const RECT& rect, CWnd* pParentWnd, UINT nID, 
						CCreateContext* pContext = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); 
	}

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
				UINT nID, CFile* pPersist = NULL, BOOL bStorage = FALSE,
				BSTR bstrLicKey = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); 
	}

// 属性
public:


// 操作
public:

// IHTAgentObj

// Functions
//

	long InitTrans(LPCTSTR kkbh, LPCTSTR fxlx, long cdh, LPCTSTR info, LPCTSTR hostport)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_I4 VTS_BSTR VTS_BSTR ;
		InvokeHelper(0x1, DISPATCH_METHOD, VT_I4, (void*)&result, parms, kkbh, fxlx, cdh, info, hostport);
		return result;
	}
	long WriteVehicleInfo(LPCTSTR kkbh, LPCTSTR fxlx, long cdh, LPCTSTR hphm, LPCTSTR hpzl, LPCTSTR gcsj, long clsd, long clxs, LPCTSTR wfdm, long cwkc, LPCTSTR hpys, LPCTSTR cllx, LPCTSTR fzhpzl, LPCTSTR fzhphm, LPCTSTR fzhpys, LPCTSTR clpp, LPCTSTR clwx, LPCTSTR csys, LPCTSTR tplj, LPCTSTR tp1, LPCTSTR tp2, LPCTSTR tp3, LPCTSTR tztp)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_I4 VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4 VTS_I4 VTS_BSTR VTS_I4 VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR ;
		InvokeHelper(0x2, DISPATCH_METHOD, VT_I4, (void*)&result, parms, kkbh, fxlx, cdh, hphm, hpzl, gcsj, clsd, clxs, wfdm, cwkc, hpys, cllx, fzhpzl, fzhphm, fzhpys, clpp, clwx, csys, tplj, tp1, tp2, tp3, tztp);
		return result;
	}
	long SaveTzPicture(LPCTSTR kkbh, LPCTSTR fxlx, long cdh, LPCTSTR hphm, LPCTSTR gcsj, LPCTSTR file)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_I4 VTS_BSTR VTS_BSTR VTS_BSTR ;
		InvokeHelper(0x3, DISPATCH_METHOD, VT_I4, (void*)&result, parms, kkbh, fxlx, cdh, hphm, gcsj, file);
		return result;
	}
	long SavePicture(LPCTSTR kkbh, LPCTSTR fxlx, long cdh, LPCTSTR hphm, LPCTSTR gcsj, long tpxh, LPCTSTR file)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_I4 VTS_BSTR VTS_BSTR VTS_I4 VTS_BSTR ;
		InvokeHelper(0x4, DISPATCH_METHOD, VT_I4, (void*)&result, parms, kkbh, fxlx, cdh, hphm, gcsj, tpxh, file);
		return result;
	}
	long SaveTzPicStream(LPCTSTR kkbh, LPCTSTR fxlx, long cdh, LPCTSTR hphm, LPCTSTR gcsj, unsigned char * pbuf, long size)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_I4 VTS_BSTR VTS_BSTR VTS_PUI1 VTS_I4 ;
		InvokeHelper(0x5, DISPATCH_METHOD, VT_I4, (void*)&result, parms, kkbh, fxlx, cdh, hphm, gcsj, pbuf, size);
		return result;
	}
	long SavePicStream(LPCTSTR kkbh, LPCTSTR fxlx, long cdh, LPCTSTR hphm, LPCTSTR gcsj, long tpxh, unsigned char * pbuf, long size)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_I4 VTS_BSTR VTS_BSTR VTS_I4 VTS_PUI1 VTS_I4 ;
		InvokeHelper(0x6, DISPATCH_METHOD, VT_I4, (void*)&result, parms, kkbh, fxlx, cdh, hphm, gcsj, tpxh, pbuf, size);
		return result;
	}
	long QueryLimitSpeed(LPCTSTR kkbh, LPCTSTR fxlx, long cdh, LPCTSTR cllx)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_I4 VTS_BSTR ;
		InvokeHelper(0x7, DISPATCH_METHOD, VT_I4, (void*)&result, parms, kkbh, fxlx, cdh, cllx);
		return result;
	}
	CString QuerySyncTime()
	{
		CString result;
		InvokeHelper(0x8, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	CString GetLastMessage()
	{
		CString result;
		InvokeHelper(0x9, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
		return result;
	}

// Properties
//



};
